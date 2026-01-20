//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* hitgroup
#include "HitgroupCommon.hlsli"

//* content
#include "../../../../Content/Material.hlsli"

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
// anyhit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
ANYHIT void mainAnyhit(inout Payload payload, in Attribute attribute) {

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
CLOSESTHIT void mainClosesthit(inout Payload payload, in Attribute attribute) {

	Surface surface;
	surface.GetSurface(attribute);

	payload.albedo   = surface.albedo;
	payload.material = float3(surface.ao, surface.roughness, surface.metallic);
	payload.normal   = surface.normal;
	payload.position = surface.position;

	payload.isHit = true;
	
}
