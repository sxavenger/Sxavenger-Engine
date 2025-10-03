//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* hitgroup
#include "HitgroupCommon.hlsli"

//* content
#include "../../../Content/Material.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

StructuredBuffer<Material> gMaterial : register(t0);
//!< FIXME: ConstantBufferで使えるようにする. 多分alignment問題
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// Surface structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Surface {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float3 position;
	float3 normal;
	float3 albedo;
	float ao;
	float roughness;
	float metallic;

	//=========================================================================================
	// public methods
	//=========================================================================================

	void GetSurface(Attribute attribute) {
		
		MeshVertex vertex = GetWorldVertex(attribute);

		MaterialLib::TextureSampler parameter;
		parameter.Set(vertex.texcoord, gSampler);

		albedo   = gMaterial[0].albedo.GetAlbedo(parameter);
		position = vertex.position.xyz;

		float3x3 tbn = float3x3(
			vertex.tangent,
			vertex.bitangent,
			vertex.normal
		);
		normal = gMaterial[0].normal.GetNormal(vertex.normal, parameter, tbn);

		ao        = gMaterial[0].properties.ao.GetValue(parameter, 0);
		roughness = gMaterial[0].properties.roughness.GetValue(parameter, 1);
		metallic  = gMaterial[0].properties.metallic.GetValue(parameter, 2);

		roughness = max(roughness, 0.02f); //!< 0.0fだと計算が不安定になるので、最低値を設定する
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 CalculateDirectionalLight(uint index, Surface surface) {
	//* Lightの情報を取得
	float3 color_mask = gDirectionalLights[index].GetColorMask();
	float light_mask  = gDirectionalLights[index].GetLightMask(gScene, gDirectionalLightTransforms[index].GetDirection(), surface.position);
	float3 l          = gDirectionalLights[index].GetDirectionFromSurface(gDirectionalLightTransforms[index].GetDirection()); //!< lightの方向ベクトル

	//* Viewの情報を取得
	float3 v = normalize(-WorldRayDirection()); //!< viewからの方向ベクトルを取得

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

	return (diffuseBRDF + specularBRDF) * NdotL * color_mask * light_mask;
}

float3 CalculatePointLight(uint index, Surface surface) {

	//* Lightの情報を取得
	float3 color_mask = gPointLights[index].GetColorMask();
	float light_mask  = gPointLights[index].GetLightMask(gScene, gPointLightTransforms[index].GetPosition(), surface.position);
	float3 l          = gPointLights[index].GetDirectionFromSurface(gPointLightTransforms[index].GetPosition(), surface.position); //!< lightの方向ベクトル

	//* Viewの情報を取得
	float3 v = normalize(-WorldRayDirection()); //!< viewからの方向ベクトルを取得

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

	float3 f  = F_SphericalGaussian(VdotH, specularAlbedo);
	float  vh = V_HeightCorrelated(NdotV, NdotL, surface.roughness);
	float  d  = D_GGX(NdotH, surface.roughness);

	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	float3 specularBRDF = SpecularBRDF(f, vh, d);

	return (diffuseBRDF + specularBRDF) * NdotL * color_mask * light_mask;
	
}

float3 CalculateSpotLight(uint index, Surface surface) {

	//* Lightの情報を取得
	float3 color_mask = gSpotLights[index].GetColorMask();
	float light_mask  = gSpotLights[index].GetLightMask(gScene, gSpotLightTransforms[index].GetPosition(), gSpotLightTransforms[index].GetDirection(), surface.position);
	float3 l          = gSpotLights[index].GetDirectionFromSurface(gSpotLightTransforms[index].GetPosition(), surface.position); //!< lightの方向ベクトル

	//* Viewの情報を取得
	float3 v = normalize(-WorldRayDirection()); //!< viewからの方向ベクトルを取得

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

	float3 f  = F_SphericalGaussian(VdotH, specularAlbedo);
	float  vh = V_HeightCorrelated(NdotV, NdotL, surface.roughness);
	float  d  = D_GGX(NdotH, surface.roughness);

	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	float3 specularBRDF = SpecularBRDF(f, vh, d);

	return (diffuseBRDF + specularBRDF) * NdotL * color_mask * light_mask;
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// anyhit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_ANYHIT void mainAnyhit(inout Payload payload, in Attribute attribute) {

	MeshVertex vertex = GetWorldVertex(attribute);

	MaterialLib::TextureSampler parameter;
	parameter.Set(vertex.texcoord, gSampler);
	
	float transparency = gMaterial[0].transparency.GetTransparency(parameter);
	if (transparency <= 0.1f) {
		IgnoreHit(); //!< 透明度が低い場合は、ヒットを無視する
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// closesthit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_CLOSESTHIT void mainClosesthit(inout Payload payload, in Attribute attribute) {

	Surface surface;
	surface.GetSurface(attribute);

	float3 indirect = float3(0.0f, 0.0f, 0.0f);

	for (uint i = 0; i < gDirectionalLightCount.count; ++i) {
		indirect += CalculateDirectionalLight(i, surface);
	}

	for (uint i = 0; i < gPointLightCount.count; ++i) {
		indirect += CalculatePointLight(i, surface);
	}

	for (uint i = 0; i < gSpotLightCount.count; ++i) {
		indirect += CalculateSpotLight(i, surface);
	}

	payload.indirect = float4(indirect, 1.0f);
	
}
