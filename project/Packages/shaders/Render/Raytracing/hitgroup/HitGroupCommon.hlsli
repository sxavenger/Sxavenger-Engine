#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../RaytracingCommon.hlsli"
#include "../../../VertexStructure.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

StructuredBuffer<Vertex> gVertices : register(t10);
StructuredBuffer<uint>   gIndices  : register(t11);

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

Vertex GetLocalVertex(Attribute attribute) {

	const float3 barycentric = attribute.GetBarycentrics();
	uint vertexId            = PrimitiveIndex() * 3;

	Vertex vertex = (Vertex)0;

	for (uint i = 0; i < 3; ++i) {
		uint index   = gIndices[vertexId + i];
		float weight = barycentric[i];

		vertex.position  += gVertices[index].position  * weight;
		vertex.texcoord  += gVertices[index].texcoord  * weight;
		vertex.normal    += gVertices[index].normal    * weight;
		vertex.bitangent += gVertices[index].bitangent * weight;
		vertex.tangent   += gVertices[index].tangent   * weight;
	}

	vertex.normal    = normalize(vertex.normal);
	//vertex.bitangent = normalize(vertex.bitangent);
	//vertex.tangent   = normalize(vertex.tangent);

	return vertex;
}

Vertex GetWorldVertex(Attribute attribute) {
	
	Vertex vertex = GetLocalVertex(attribute);
	
	vertex.position = mul(float4(vertex.position.xyz, 1.0f), ObjectToWorld4x3()).xyz;
	vertex.normal   = mul(vertex.normal, (float3x3)ObjectToWorld4x3());
	
	return vertex;
	
}
