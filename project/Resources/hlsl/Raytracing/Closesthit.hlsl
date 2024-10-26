//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RaytracingCommon.hlsli"

//=========================================================================================
// LocalBuffer
//=========================================================================================

StructuredBuffer<Vertex> gVertices : register(t0);
StructuredBuffer<uint>   gIndices : register(t1);

Texture2D<float4> gAlbedo : register(t2);
SamplerState     gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

Vertex GetHitVertex(Attribute attribute) {
	
	Vertex result = (Vertex)0;
	float3 barycentrics = CalcBarycentrics(attribute.barys);
	uint vertexId       = PrimitiveIndex() * 3;
	
	float weights[3] = {
		barycentrics.x, barycentrics.y, barycentrics.z
	};
	
	for (int i = 0; i < 3; ++i) {
		uint index = gIndices[vertexId + i];
		float w    = weights[i];
		
		result.position += gVertices[index].position * w;
		result.texcoord += gVertices[index].texcoord * w;
		result.normal   += gVertices[index].normal * w;
	}
	
	result.normal = normalize(result.normal);
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// entry point closesthit
////////////////////////////////////////////////////////////////////////////////////////////
[shader("closesthit")]
void mainClosesthit(inout Payload payload, Attribute attribute) {

	Vertex vertex = GetHitVertex(attribute);

	float4 color = gAlbedo.SampleLevel(gSampler, vertex.texcoord, 0.0f);
	
	payload.color = color;

}