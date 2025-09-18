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
	float3 c_light = gDirectionalLights[index].GetColor();               //!< lightのcolor
	float3 l       = -gDirectionalLightTransforms[index].GetDirection(); //!< surfaceからlightへの方向ベクトル

	//* shadow
	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = l;
	desc.TMin      = kTMin;
	desc.TMax      = kTMax;

	c_light *= gDirectionalLights[index].TraceShadow(desc, gScene);
	// todo: 不必要な場合は、gShadow.TraceShadow()を呼び出さないようにする

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);

	//* 計算
	float3 h = normalize(l + v);

	float NdotV = saturate(dot(surface.normal, v));
	float NdotL = saturate(dot(surface.normal, l));
	float NdotH = saturate(dot(surface.normal, h));
	float VdotH = saturate(dot(v, h));

	// f0
	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0

	// diffuse Albedo
	//!< 金属(metallic = 1.0f) -> 0.0f
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - f0)
	//float3 diffuseAlbedo = surface.albedo * (1.0f - f0) * (1.0f - surface.metallic);
	float3 diffuseAlbedo = surface.albedo * (1.0f - f0) * (1.0f - surface.metallic);

	// specular Albedo
	//!< 金属(metallic = 1.0f) -> f0
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 specularAlbedo = lerp(f0, surface.albedo, surface.metallic);

	float3 f = F_SphericalGaussian(VdotH, specularAlbedo);
	float vh = V_HeightCorrelated(NdotV, NdotL, surface.roughness);
	float d  = D_GGX(NdotH, surface.roughness);

	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	float3 specularBRDF = SpecularBRDF(f, vh, d);

	return (diffuseBRDF + specularBRDF) * NdotL * c_light;
}

float3 CalculatePointLight(uint index, Surface surface) {

	//* Lightの情報を取得
	float3 p_light = gPointLightTransforms[index].GetPosition(); //!< lightの中心座標
	float3 l       = normalize(p_light - surface.position);      //!< lightの方向ベクトル
	float r        = length(p_light - surface.position);         //!< lightとsurfaceの距離

	float3 c_light = gPointLights[index].GetColor(r);

	// 影の計算
	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = l;
	desc.TMin      = kTMin;
	desc.TMax      = r;
	
	c_light *= gPointLights[index].TraceShadow(desc, gScene);

	//* Cameraの情報を取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);

	//* 計算
	float3 h = normalize(l + v);

	float NdotV = saturate(dot(surface.normal, v));
	float NdotL = saturate(dot(surface.normal, l));
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

	return (diffuseBRDF + specularBRDF) * NdotL * c_light;
	
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

	payload.indirect.rgb = float3(0.0f, 0.0f, 0.0f);
	payload.indirect.a   = 1.0f;

	for (uint i = 0; i < gDirectionalLightCount.count; ++i) {
		payload.indirect.rgb += CalculateDirectionalLight(i, surface);
	}

	for (uint i = 0; i < gPointLightCount.count; ++i) {
		payload.indirect.rgb += CalculatePointLight(i, surface);
	}
	
}
