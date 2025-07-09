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
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 CalculateDirectionalLight(uint index, float3 position, float3 normal, float3 albedo, float roughness, float metallic) {
	//* Lightの情報を取得
	float3 c_light = gDirectionalLights[index].GetColor(); //!< lightのcolor
	float3 l       = -gDirectionalLightTransforms[index].GetDirection(); //!< surfaceからlightへの方向ベクトル

	//* shadow
	RayDesc desc;
	desc.Origin    = position;
	desc.Direction = l;
	desc.TMin      = kTMin;
	desc.TMax      = kTMax;

	c_light *= gDirectionalLightShadows[index].TraceShadow(desc, gScene);
	// todo: 不必要な場合は、gShadow.TraceShadow()を呼び出さないようにする

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - position);

	//* 計算
	float3 h = normalize(l + v);

	float NdotV = saturate(dot(normal, v));
	float NdotL = saturate(dot(normal, l));
	float NdotH = saturate(dot(normal, h));
	float VdotH = saturate(dot(v, h));

	// f0
	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0

	// diffuse Albedo
	//!< 金属(metallic = 1.0f) -> 0.0f
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - f0)
	//float3 diffuseAlbedo = surface.albedo * (1.0f - f0) * (1.0f - surface.metallic);
	float3 diffuseAlbedo = albedo * (1.0f - f0) * (1.0f - metallic);

	// specular Albedo
	//!< 金属(metallic = 1.0f) -> f0
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 specularAlbedo = lerp(f0, albedo, metallic);

	float3 f = F_SphericalGaussian(VdotH, specularAlbedo);
	float vh = V_HeightCorrelated(NdotV, NdotL, roughness);
	float d  = D_GGX(NdotH, roughness);

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

	MeshVertex vertex = GetWorldVertex(attribute);

	MaterialLib::TextureSampler parameter;
	parameter.Set(vertex.texcoord, gSampler);

	float3 albedo   = gMaterial[0].albedo.GetAlbedo(parameter);

	float3x3 tbn = float3x3(
		vertex.tangent.xyz,
		vertex.bitangent.xyz,
		vertex.normal.xyz
	);
	float3 normal = gMaterial[0].normal.GetNormal(vertex.normal, parameter, tbn);
	
	float metallic  = gMaterial[0].properties.metallic.GetValue(parameter, 2);
	float roughness = gMaterial[0].properties.roughness.GetValue(parameter, 1);
	// FIXME: materialの値が違う

	for (uint i = 0; i < gDirectionalLightCount.count; ++i) {
		payload.color.rgb += CalculateDirectionalLight(i, vertex.position.xyz, normal, albedo, roughness, metallic);
	}

	payload.color.a = 1.0f;

	if (payload.rayType != RayType::kPrimary) {
		return;
	}

	//Payload reflection;
	//reflection.Reset();
	//reflection.rayType = RayType::kReflection;

	//RayDesc desc;
	//desc.Origin    = vertex.position.xyz;
	//desc.Direction = reflect(WorldRayDirection(), normal); //!< 反射ベクトルを計算
	//desc.TMax      = kTMax;
	//desc.TMin      = kTMin;

	//payload.TraceRecursionRay(reflection, desc);

	//payload.color.rgb += reflection.color.rgb * (1.0f - roughness); // FIXME: 仮

	payload.SetPrimaryParameter(vertex.position.xyz, normal, 0.0f, roughness, metallic);
	
}
