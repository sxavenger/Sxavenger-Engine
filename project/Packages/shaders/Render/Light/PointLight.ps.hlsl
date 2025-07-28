//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* component
#include "../../Component/PointLightComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

StructuredBuffer<PointLightComponent> gParameters : register(t0);
StructuredBuffer<InlineShadow> gShadows           : register(t1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);

	//* Lightの情報を取得
	float3 p_light = gTransforms[input.instanceId].GetPosition(); //!< lightの中心座標
	float3 l       = normalize(p_light - surface.position);       //!< lightの方向ベクトル
	float r        = length(p_light - surface.position);          //!< lightとsurfaceの距離

	float3 c_light = gParameters[input.instanceId].GetColor(r);

	// 影の計算
	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = l;
	desc.TMin      = kTMin;
	desc.TMax = r;
	
	c_light *= gShadows[input.instanceId].TraceShadow(desc, gScene);

	//* Cameraの情報を取得
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

	float3 f  = F_SphericalGaussian(VdotH, specularAlbedo);
	float  vh = V_HeightCorrelated(NdotV, NdotL, surface.roughness);
	float  d  = D_GGX(NdotH, surface.roughness);

	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	float3 specularBRDF = SpecularBRDF(f, vh, d);

	output.color.rgb = (diffuseBRDF + specularBRDF) * NdotL * c_light;

	output.color.a = 1.0f;
	return output;
	
}


