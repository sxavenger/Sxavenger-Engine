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
	float roughness;
	float metallic;

	//=========================================================================================
	// public methods
	//=========================================================================================

	void GetSurface(Attribute attribute) {
		
		MeshVertex vertex = GetWorldVertex(attribute);

		MaterialLib::TextureSampler parameter
			= gMaterial[0].CreateTransformedSampler(vertex.texcoord, gSampler);

		albedo   = gMaterial[0].GetAlbedo(parameter);
		position = vertex.position.xyz;
		normal   = gMaterial[0].GetNormal(vertex.normal, vertex.tangent, vertex.bitangent, parameter);

		roughness = gMaterial[0].GetRoughness(parameter);
		metallic  = gMaterial[0].GetMetallic(parameter);
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
// anyhit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
ANYHIT void mainEmissiveAnyhit(inout Payload payload, in Attribute attribute) {

	MeshVertex vertex = GetWorldVertex(attribute);

	MaterialLib::TextureSampler parameter
		= gMaterial[0].CreateTransformedSampler(vertex.texcoord, gSampler);
	
	float transparency = gMaterial[0].GetTransparency(parameter);
	if (transparency <= 0.1f) {
		IgnoreHit(); //!< 透明度が低い場合は、ヒットを無視する
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// closesthit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
CLOSESTHIT void mainEmissiveClosesthit(inout Payload payload, in Attribute attribute) {

	Surface surface;
	surface.GetSurface(attribute);

	payload.lo       = surface.albedo; //!< TODO: Emissiveの色を取得する必要がある
	payload.position = surface.position;
	payload.normal   = surface.normal;
	
}
