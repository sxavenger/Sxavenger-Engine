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

	MaterialLib::TextureParameter parameter;
	parameter.Set(vertex.texcoord, gSampler);
	
	payload.color.rgb = gMaterial.albedo.GetAlbedo(parameter);
	payload.color.a   = 1.0f;

	float4 d = mul(vertex.position, kViewProj);
	payload.depth = d.z / d.w;
	
}
