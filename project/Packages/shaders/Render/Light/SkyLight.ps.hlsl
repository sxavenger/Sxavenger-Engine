//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* library
#include "../../Library/BRDF.hlsli"

#define _DEBUG_SAMPLE

#ifdef _DEBUG_SAMPLE
#include "../../Library/Hammersley.hlsli"
#endif

//=========================================================================================
// buffers
//=========================================================================================

SamplerState gSampler : register(s0);

struct DiffuseParameter {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint index;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float4 Sample(SamplerState sample, float3 v) {
		TextureCube<float4> texture = ResourceDescriptorHeap[index];
		return texture.SampleLevel(sample, v, 0);
	}
	
};
ConstantBuffer<DiffuseParameter> gDiffuseParameter : register(b0);

struct SpecularParameter {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint index;
	uint miplevels;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float4 Sample(SamplerState sample, float3 v, float lod) {
		TextureCube<float4> texture = ResourceDescriptorHeap[index];
		return texture.SampleLevel(sample, v, lod);
	}
	
};
ConstantBuffer<SpecularParameter> gSpecularParameter : register(b1);

Texture2D<float4> gBRDFLut : register(t0);
SamplerState gBRDFSampler  : register(s1);

ConstantBuffer<InlineShadow> gShadow : register(b2);

#ifdef _DEBUG_SAMPLE
TextureCube<float4> gEnvironment : register(t2, space1);
static const uint kSampleCount = 1024; //!< サンプル数
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG_SAMPLE

float3 ImportanceSampleGGX(float2 xi, float roughness, float3 n) {
	float a = roughness * roughness;

	float phi = kTau * xi.x;
	float cosTheta = sqrt((1.0f - xi.y) / (1.0f + (a * a - 1.0f) * xi.y));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	float3 h = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta); //!< 半径1の円周上にサンプリング

	float3 up = abs(n.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
	float3 tangentX = normalize(cross(up, n)); //!< 接線ベクトル
	float3 tangentY = cross(n, tangentX); //!< 接線ベクトル

	return normalize(h.x * tangentX + h.y * tangentY + h.z * n);
}

float3 PrefilterRadiance(float roughness, float3 r) {
	// 観測者が物体の真上から見下ろしていることを前提(v = n = r);
	float3 n = r;
	float3 v = r;

	float3 color = 0.0f;
	float totalWeight = 0.0f;
	
	for (uint i = 0; i < kSampleCount; ++i) {
		float2 xi = Hammersley(i, kSampleCount);
		float3 h = ImportanceSampleGGX(xi, roughness, n); //!< 法線ベクトルをサンプリング
		float3 l = 2.0f * dot(v, h) * h - v; //!< ライトベクトルを計算

		float NdotL = saturate(dot(n, l)); //!< ライトベクトルと法線ベクトルの内積

		if (NdotL > 0.0f) {
			color += NdotL * gEnvironment.SampleLevel(gSampler, l, 0).rgb; //!< 環境マップをサンプリング
			totalWeight += NdotL;
		}
	}

	return color / totalWeight;
}

float3 ImportanceSampleCosineWieghted(float2 xi, float3 n) {
	float r = sqrt(xi.x);
	float phi = kTau * xi.y;

	float3 h = float3(r * cos(phi), r * sin(phi), sqrt(1.0f - xi.x)); //!< 半径1の円周上にサンプリング

	float3 up = abs(n.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f); //!< 上方向を決定
	float3 tangentX = normalize(cross(up, n)); //!< 接線ベクトル
	float3 tangentY = cross(n, tangentX); //!< 接線ベクトル

	return normalize(h.x * tangentX + h.y * tangentY + h.z * n); //!< 法線ベクトルに変換
}

float3 PrefilterIrradiance(float3 n) {

	float3 color = 0.0f;

	for (uint i = 0; i < kSampleCount; i++) {
		float2 xi = Hammersley(i, kSampleCount);
		float3 h = ImportanceSampleCosineWieghted(xi, n); //!< 半径1の円周上にサンプリング

		color += gEnvironment.SampleLevel(gSampler, h, 0.0f).rgb; //!< 環境マップから色を取得
	}

	return color / float(kSampleCount); //!< 平均化
}

#endif

float3 ApproximateBRDF(float3 diffuseAlbedo, float3 specularAlbedo, float3 n, float3 v, float3 r, float roughness) {

	float NdotV = saturate(dot(n, v));

	const float2 brdf = gBRDFLut.SampleLevel(gBRDFSampler, saturate(float2(NdotV, 1.0f - roughness)), 0).rg;

	float lod = roughness * float(gSpecularParameter.miplevels);

#ifdef _DEBUG_SAMPLE
	float3 diffuseLight  = PrefilterIrradiance(n);
	float3 specularLight = PrefilterRadiance(roughness, r);
#else
	float3 diffuseLight  = gDiffuseParameter.Sample(gSampler, n).rgb;
	float3 specularLight = gSpecularParameter.Sample(gSampler, r, lod).rgb;
#endif
	
	float3 diffuse = diffuseLight * diffuseAlbedo;
	float3 specular = specularLight * (specularAlbedo * brdf.r + brdf.g);

	return /*diffuse +*/ specular;

}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);

	//* Cameraの情報を取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);
	float3 r = reflect(-v, surface.normal);

	// f0
	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0

	// diffuse Albedo
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - f0)
	//!< 金属(metallic = 1.0f) -> 0.0f
	float3 diffuseAlbedo = surface.albedo * (1.0f - f0) * (1.0f - surface.metallic);

	// specular Albedo
	//!< 非金属(metallic = 0.0f) -> albedo
	//!< 金属(metallic = 1.0f) -> f0
	float3 specularAlbedo = lerp(f0, surface.albedo, surface.metallic);

	output.color.rgb = ApproximateBRDF(diffuseAlbedo, specularAlbedo, surface.normal, v, r, surface.roughness);

	//* 影の計算
	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = r;
	desc.TMin      = kTMin;
	desc.TMax      = kTMax;
	
	//output.color.rgb *= gShadow.TraceShadow(desc);

	output.color.a = 1.0f;
	return output;
}
