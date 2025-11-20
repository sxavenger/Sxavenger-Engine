//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PathtracingCommon.hlsli"

//* library
#include "../../Library/BRDF.hlsli"
#include "../../Library/ACES.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_RAYGENERATION void mainRaygeneration() {

	uint2 index     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	uint p = index.x + index.y * dimension.x; //!< 1次元配列のindex

	Moment moment = gMoment[p];

	moment.CheckOffset(index);

	Reservoir reservoir = (Reservoir)0;

	DeferredSurface surface;
	if (!surface.GetSurface(gDeferredBufferIndex.Get(), index)) {
		gInitalizeReservoir[p] = reservoir;
		gMoment[p]             = Moment::GetDefault(index);
		return; //!< surfaceが存在しない
	}

	if (moment.CheckCompleteSample(maxSampleCount)) {
		gInitalizeReservoir[p] = reservoir;
		return; //!< サンプリング終了
	}

	Random random;
	random.seed = seed * uint3(index + 1, 1);

	static const float3 kMinFrenel = float3(0.04f, 0.04f, 0.04f); //!< 非金属の最小Frenel値

	for (uint i = 0; i < samplesPerFrame; ++i) {
		//!< XXX: samplesPerFrameが1じゃないとオーバーランする.

		//!< cosine weighted hemisphere sampling で実装.
		float2 xi = Hammersley(moment.GetRandamizeSampleIndex(i, maxSampleCount), maxSampleCount);
		float3 wi = ImportanceSampleLambert(xi, surface.normal);

		float pdf = ImportanceSampleLambertPDF(wi, surface.normal);
		
		RayDesc desc;
		desc.Origin    = surface.position;
		desc.Direction = wi;
		desc.TMin      = kTMin;
		desc.TMax      = kTMax;

		Payload payload = TracePrimaryRay(desc, kFlag);

		//* Lightの情報を取得
		float3 l = wi; //!< lightの方向ベクトル

		//* cameraからの方向ベクトルを取得
		float3 v = normalize(gCamera.GetPosition() - surface.position); //!< cameraからの方向ベクトルを取得

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
		
		float3 color = (diffuseBRDF + specularBRDF) * NdotL * payload.lo;

		Sample sample;
		sample.lo  = color;
		sample.pdf = pdf;

		float w = dot(sample.lo, ACES::AP1_RGB2Y) * rcp(max(pdf, kEpsilon));

		reservoir.Update(sample, w, random.Generate1d());
		
	}

	//!< reservoirの正規化
	reservoir.weight *= rcp(max(reservoir.m, 1.0f));
	gInitalizeReservoir[p] = reservoir;

	//!< momentの更新
	moment.index += samplesPerFrame;
	gMoment[p]    = moment;
	
}
