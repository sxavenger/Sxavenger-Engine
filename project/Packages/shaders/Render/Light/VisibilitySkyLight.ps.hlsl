//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* library
#include "../../Library/BRDF.hlsli"
#include "../../Library/ACES.hlsli"
#include "../../Library/RandomLib.hlsli"
#include "../../Library/ImportanceSample.hlsli"

//* content
#include "../../Content/Random.hlsli"

//* component
#include "../../Component/SkyLightComponent.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Sample structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Sample {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float3 lo; //!< sample radiance
	float3 wi;
	float pdf;
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// Reservoir structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Reservoir {

	//=========================================================================================
	// public variables
	//=========================================================================================

	Sample sample;
	float weight; //!< wsum
	float w;      //!< W
	uint m;       //!< M

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief sample update
	//! @param s[in] new sample
	//! @param w[in] new sample weight
	//! @param r[in] random value [0, 1]
	void Update(Sample s, float w, float r) {
		weight += w;
		m++;
		
		if (r < w * rcp(weight)) {
			sample = s;
		}

	}

	//! @brief reservoir merge
	//! @param reservoir[in] merge source reservoir
	//! @param p_hat[in] estimated sample weight of the target reservoir
	//! @param r[in] random value [0, 1]
	void Merge(Reservoir reservoir, float p_hat, float r) {
		uint m0 = m;
		Update(reservoir.sample, p_hat * reservoir.w * reservoir.m, r);
		m = m0 + reservoir.m;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// Moment structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Moment {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint offset;
	uint index;

	//=========================================================================================
	// public methods
	//=========================================================================================

	static Moment GetDefault(uint2 pixel) {
		Moment moment = (Moment)0;
		moment.offset = Xorshift::xorshift32(pixel.x * pixel.y) + 1;

		return moment;
	}

	uint GetRandamizeSampleIndex(uint i, uint maxSampleCount) {
		const uint kDivision = maxSampleCount / 16;
		uint divisionIndex = kDivision * ((index + i + offset) % maxSampleCount);
	
		return (divisionIndex % maxSampleCount) + (divisionIndex / maxSampleCount);
	}

	bool CheckCompleteSample(uint maxSampleCount) {
		return index >= maxSampleCount;
	}
	
};

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<SkyLightComponent> gParameter : register(b0);
SamplerState gSampler : register(s0);

cbuffer Dimension : register(b1) {
	uint2 dimension;
}

cbuffer Seed : register(b2) {
	uint3 seed;
}

RWStructuredBuffer<Reservoir> gReservoir : register(u0);
RWStructuredBuffer<Moment> gMoment       : register(u1);

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

//! @brief 2d Pixel To 1d Index
uint Flatten(uint2 pixel) {
	return pixel.y * dimension.x + pixel.x;
}

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 TraceRayEnvironment(RayDesc desc, uint flag) {
	
#ifdef _INLINE_RAYTRACING
		RayQuery<0> q;

		q.TraceRayInline(
			gScene,
			flag,
			0xFF,
			desc
		);

		if (q.Proceed()) {
			return 0.0f;
		}
#endif

	return gParameter.GetEnvironment(gSampler, desc.Direction).rgb;
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {
	
	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);

	Random random;
	random.seed = seed * uint3(input.position.xy + 1, 1);

	//!< cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);

	uint p = Flatten(input.position.xy); //!< 1次元配列のindex

	Moment moment       = gMoment[p];
	Reservoir reservoir = gReservoir[p];

	static const uint kMaxSampleCount = 128;

	if (moment.index < kMaxSampleCount) { //!< Initialize and Temporal Reuse.

		Sample sample;

		float2 xi = Hammersley(moment.GetRandamizeSampleIndex(0, kMaxSampleCount), 1);
		moment.index++;

		//!< GGX Importance Sample を使用
		sample.wi  = ImportanceSampleGGX(xi, surface.roughness, surface.normal);
		sample.pdf = ImportanceSampleGGXPDF(sample.wi, surface.roughness, surface.normal, v);

		RayDesc desc;
		desc.Origin    = surface.position;
		desc.Direction = sample.wi;
		desc.TMin      = kTMin;
		desc.TMax      = kTMax;

		sample.lo = TraceRayEnvironment(desc, RAY_FLAG_CULL_BACK_FACING_TRIANGLES);
		
		float p_hat = dot(sample.lo, ACES::AP1_RGB2Y);
		float p_q   = sample.pdf;

		float w = p_q > 0.0f ? p_hat * rcp(p_q) : 0.0f;

		reservoir.Update(sample, w, random.Generate1d());

		p_hat = dot(reservoir.sample.lo, ACES::AP1_RGB2Y);

		reservoir.w = (p_hat > 0.0 && reservoir.m > 0) ? reservoir.weight * rcp(reservoir.m * p_hat) : 0.0f;

		gMoment[Flatten(input.position.xy)]    = moment;
		gReservoir[Flatten(input.position.xy)] = reservoir;
	}

	//* Lightの情報を取得
	float3 l = reservoir.sample.wi; //!< lightの方向ベクトル

	//* 計算
	float3 h = normalize(l + v);

	float NdotV = saturate(dot(surface.normal, v));
	float NdotL = saturate(dot(surface.normal, l));
	float NdotH = saturate(dot(surface.normal, h));
	float VdotH = saturate(dot(v, h));

	static const float3 kMinFrenel = float3(0.04f, 0.04f, 0.04f); //!< 非金属の最小Frenel値

	// diffuse Albedo
	//!< 金属(metallic = 1.0f) -> 0.0f
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - kMinFrenel)
	float3 diffuseAlbedo = surface.albedo * (1.0f - kMinFrenel) * (1.0f - surface.metallic);

	// specular Albedo
	//!< 金属(metallic = 1.0f) -> kMinFrenel
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 specularAlbedo = lerp(kMinFrenel, surface.albedo, surface.metallic);

	float3 f = F_SphericalGaussian(VdotH, specularAlbedo);
	float vh = V_HeightCorrelated(NdotV, NdotL, surface.roughness);
	float d  = D_GGX(NdotH, surface.roughness);

	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	float3 specularBRDF = SpecularBRDF(f, vh, d);
	
	output.color.rgb = (diffuseBRDF + specularBRDF) * reservoir.sample.lo * reservoir.w;

	output.color.a = 1.0f;
	return output;

	
}

