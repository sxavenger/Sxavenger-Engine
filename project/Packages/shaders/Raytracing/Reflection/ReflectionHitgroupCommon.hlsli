#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ReflectionRaytracing.hlsli"
#include "../../VertexStructure.hlsli"

//=========================================================================================
// buffers
//=========================================================================================
//* local buffer (hitgroup global)

StructuredBuffer<Vertex> gVertices : register(t0);
StructuredBuffer<uint>   gIndices  : register(t1);

//!< todo: texture conponent

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 CalcBarycentrics(float2 barys) {
	return float3(
		1.0f - barys.x - barys.y,
		barys.x,
		barys.y
	);
}

Vertex GetVertex(Attribute attribute) {
	
	float3 barycentrics = CalcBarycentrics(attribute.barys);
	uint vertexId       = PrimitiveIndex() * 3;
	
	Vertex vtx = (Vertex)0;
	
	for (uint i = 0; i < 3; ++i) {
		uint index = gIndices[vertexId + i];
		float w    = barycentrics[i];

		vtx.position  += gVertices[index].position  * w;
		vtx.normal    += gVertices[index].normal    * w;
		vtx.texcoord  += gVertices[index].texcoord  * w;
		vtx.bitangent += gVertices[index].bitangent * w;
		vtx.tangent   += gVertices[index].tangent   * w;
	}
	
	vtx.normal = normalize(vtx.normal);
	return vtx;
}

Vertex ToWorld(Vertex vtx) {

	vtx.position = mul(vtx.position, ObjectToWorld4x3());
	vtx.normal   = normalize(mul(vtx.normal, (float3x3)ObjectToWorld4x3()));
	
	return vtx;
}
