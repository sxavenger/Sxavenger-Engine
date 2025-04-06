//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "HitgroupCommon.hlsli"
#include "../../Component/MaterialComponent.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

ConstantBuffer<MaterialComponent> gMaterial : register(b0);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// anyhit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
[shader("anyhit")]
void mainAnyhit(inout Payload payload, in Attribute attribute) {
}

////////////////////////////////////////////////////////////////////////////////////////////
// closesthit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
[shader("closesthit")]
void mainClosesthit(inout Payload payload, in Attribute attribute) {

	Vertex vertex = GetWorldVertex(attribute);

	Material::TextureParameter parameter;
	parameter.Set(vertex.texcoord, gSampler);
	
	payload.color = gMaterial.albedo.GetAlbedo(parameter);

	float4 d = mul(vertex.position, kViewProj);
	payload.depth = d.z / d.w;
	
}
