//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* component
#include "../../Component/DirectionalLightComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

StructuredBuffer<DirectionalLightComponent> gParameters : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;

	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);

	//* Lightの情報を取得
	float3 l          = gParameters[input.instanceId].GetDirectionFromSurface(gTransforms[input.instanceId].GetDirection()); //!< lightの方向ベクトル

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position); //!< cameraからの方向ベクトルを取得

	//* 計算
	float3 h = normalize(l + v);

	float NdotV = saturate(dot(surface.normal, v));
	float NdotL = saturate(dot(surface.normal, l));
	float NdotH = saturate(dot(surface.normal, h));
	float VdotH = saturate(dot(v, h));

	if (NdotL <= 0.0f) {
		discard;
	}

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

	//* Lightの影響範囲
	float3 color_mask = gParameters[input.instanceId].GetColorMask();
	float light_mask  = gParameters[input.instanceId].GetLightMask(gScene, gTransforms[input.instanceId].GetDirection(), surface.position);
	
	output.color.rgb = (diffuseBRDF + specularBRDF) * NdotL * color_mask * light_mask;
	// todo: specularFactorを追加

	output.color.a = 1.0f;
	return output;
	
}
