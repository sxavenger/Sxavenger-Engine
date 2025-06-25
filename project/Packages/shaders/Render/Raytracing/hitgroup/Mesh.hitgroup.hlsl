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

ConstantBuffer<Material> gMaterial : register(b0);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// anyhit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
[shader("anyhit")]
void mainAnyhit(inout Payload payload, in Attribute attribute) {
	// todo: ignore anyhit.
}

////////////////////////////////////////////////////////////////////////////////////////////
// closesthit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
[shader("closesthit")]
void mainClosesthit(inout Payload payload, in Attribute attribute) {

	MeshVertex vertex = GetWorldVertex(attribute);

	MaterialLib::TextureSampler parameter;
	parameter.Set(vertex.texcoord, gSampler);

	float3 albedo   = gMaterial.albedo.GetAlbedo(parameter);
	float metallic  = gMaterial.properties.metallic.GetValue(parameter);
	float roughness = gMaterial.properties.roughness.GetValue(parameter);

	float3 color = float3(0.0f, roughness, metallic);

	float3 v = normalize(gCamera.GetPosition() - vertex.position.xyz);

	//!< DirectionlLight
	//for (uint i = 0; i < gDirectionalLightCount.count; ++i) {
	//	float3 c_light = gDirectionalLights[i].GetColor();
	//	float3 l       = -gDirectionalLightTransforms[i].GetDirection();

	//	//* shadow
	//	RayDesc desc;
	//	desc.Origin    = vertex.position.xyz;
	//	desc.Direction = l;
	//	desc.TMin      = kTMin;
	//	desc.TMax      = kTMax;

	//	//c_light *= gDirectionalLightShadows[i].TraceShadow(desc, gScene);

	//	float3 h = normalize(l + v);

	//	float NdotV = saturate(dot(vertex.normal, v));
	//	float NdotL = saturate(dot(vertex.normal, l));
	//	float NdotH = saturate(dot(vertex.normal, h));
	//	float VdotH = saturate(dot(v, h));

	//	// f0
	//	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0

	//	// diffuse Albedo
	//	//!< 金属(metallic = 1.0f) -> 0.0f
	//	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - f0)
	//	//float3 diffuseAlbedo = surface.albedo * (1.0f - f0) * (1.0f - surface.metallic);
	//	float3 diffuseAlbedo = albedo * (1.0f - f0) * (1.0f - metallic);

	//	// specular Albedo
	//	//!< 金属(metallic = 1.0f) -> f0
	//	//!< 非金属(metallic = 0.0f) -> albedo
	//	float3 specularAlbedo = lerp(f0, albedo, metallic);

	//	float3 f = F_SphericalGaussian(VdotH, specularAlbedo);
	//	float vh = V_HeightCorrelated(NdotV, NdotL, roughness);
	//	float d  = D_GGX(NdotH, roughness);

	//	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	//	float3 specularBRDF = SpecularBRDF(f, vh, d);

	//	color += (diffuseBRDF + specularBRDF) * NdotL * c_light;
	//}

	float4 d = mul(vertex.position, kViewProj);
	payload.depth = d.z / d.w;


	payload.color = float4(color, 1.0f);
	
}
