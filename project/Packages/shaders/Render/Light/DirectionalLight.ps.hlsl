//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* component
#include "../../Component/DirectionalLightComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<DirectionalLightComponent> gParameter : register(b0);

ConstantBuffer<InlineShadow> gShadow : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;

	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);

	//* Lightの情報を取得
	float3 c_light = gParameter.GetColor(); //!< lightのcolor
	float3 l       = -gTransforms[input.instanceId].GetDirection(); //!< surfaceからlightへの方向ベクトル

	//* shadow
	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = l;
	desc.TMin      = kTMin;
	desc.TMax      = kTMax;

	c_light *= gShadow.TraceShadow(desc);
	// todo: 不必要な場合は、gShadow.TraceShadow()を呼び出さないようにする

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);

	//* 計算
	float3 h = normalize(l + v);

	float NdotV = saturate(dot(surface.normal, v)); //!< 0除算対策
	float NdotL = saturate(dot(surface.normal, l)); //!< 0除算対策
	float NdotH = saturate(dot(surface.normal, h));
	float VdotH = saturate(dot(v, h));

	// f0
	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0

	// diffuse Albedo
	//!< 金属(metallic = 1.0f) -> 0.0f
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - f0)
	float3 diffuseAlbedo = surface.albedo * (1.0f - f0) * (1.0f - surface.metallic);

	// specular Albedo
	//!< 金属(metallic = 1.0f) -> f0
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 specularAlbedo = lerp(f0, surface.albedo, surface.metallic);

	float3 f = FresnelReflectance(VdotH, specularAlbedo);
	float d = DistributionFunction(NdotH, surface.roughness);
	float g = GeometricAttenuation(NdotV, NdotL, surface.roughness);

	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo); //!< fresnel値は考慮しない方がsampleに近かった.
	float3 specularBRDF = SpecularBRDF(f, g, d, NdotL, NdotV);

	output.color.rgb = (diffuseBRDF + specularBRDF) * NdotL * c_light;

	output.color.a = 1.0f;
	return output;
	
}
