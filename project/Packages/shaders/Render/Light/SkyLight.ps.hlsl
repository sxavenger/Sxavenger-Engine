//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* library
#include "../../Library/Hammersley.hlsli"

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

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 ApproximateBRDF(float3 diffuseAlbedo, float3 specularAlbedo, float3 n, float3 v, float3 r, float roughness) {

	float NdotV = saturate(dot(n, v)) + kEpsilon;

	const float2 brdf = gBRDFLut.SampleLevel(gSampler, float2(NdotV, 1.0f - roughness), 0).rg;

	float lod = roughness * float(gSpecularParameter.miplevels);

	float3 diffuseLight  = diffuseAlbedo * gDiffuseParameter.Sample(gSampler, n).rgb;
	float3 specularLight = specularAlbedo * gSpecularParameter.Sample(gSampler, r, lod).rgb;

	float3 diffuse  = diffuseLight * diffuseAlbedo;
	float3 specular = specularLight * (specularAlbedo * brdf.r + brdf.g);

	return diffuse + specular;
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
	float3 r = -normalize(reflect(v, surface.normal));

	// f0
	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0

	surface.albedo = float3(0.0f, 0.0f, 0.0f);

	// diffuse Albedo
	//!< 金属(metallic = 1.0f) -> 0.0f
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - f0)
	float3 diffuseAlbedo = surface.albedo * (1.0f - f0) * (1.0f - surface.metallic);

	// specular Albedo
	//!< 金属(metallic = 1.0f) -> f0
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 specularAlbedo = lerp(f0, surface.albedo, surface.metallic);

	output.color.rgb = ApproximateBRDF(diffuseAlbedo, specularAlbedo, surface.normal, v, r, surface.roughness) * 12;

	output.color.a = 1.0f;
	return output;
}
