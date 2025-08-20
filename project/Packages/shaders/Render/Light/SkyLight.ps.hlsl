//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* library
#include "../../Library/BRDF.hlsli"
#include "../../Library/ACES.hlsli"

//* component
#include "../../Component/SkyLightComponent.hlsli"

#ifdef _DEBUG_SAMPLE
#include "../../Library/Hammersley.hlsli"
#endif

//=========================================================================================
// buffers
//=========================================================================================

SamplerState gSampler : register(s0);

ConstantBuffer<SkyLightComponent> gParameter : register(b0);

Texture2D<float4> gBRDFLut : register(t0);
SamplerState gBRDFSampler  : register(s1);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 ApproximateBRDF(float3 diffuseAlbedo, float3 specularAlbedo, float3 n, float3 v, float3 r, float roughness) {

	float NdotV = saturate(dot(n, v));

	const float2 brdf = gBRDFLut.SampleLevel(gBRDFSampler, saturate(float2(NdotV, 1.0f - roughness)), 0).rg;

	float3 diffuseLight  = ACES::IDT_sRGB_AP1(gParameter.GetIrradiance(gSampler, n).rgb);
	float3 specularLight = ACES::IDT_sRGB_AP1(gParameter.GetRadiance(gSampler, n, roughness).rgb);
	
	float3 diffuse = diffuseLight * diffuseAlbedo;
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

	output.color.a = 1.0f;
	return output;
}
