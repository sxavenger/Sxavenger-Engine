#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RaytracingCommon.hlsli"

//=========================================================================================
// LocalBuffer
//=========================================================================================

StructuredBuffer<Vertex> gVertices : register(t0);
StructuredBuffer<uint>   gIndices : register(t1);

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

Vertex GetHitLocalVertex(Attribute attribute) {
	
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

Vertex GetHitWorldVertex(Attribute attribute) {
	
	Vertex result = GetHitLocalVertex(attribute);

	// Local to World
	result.position.xyz = mul(result.position, ObjectToWorld4x3());
	result.normal       = normalize(mul(result.normal, (float3x3)ObjectToWorld4x3()));

	return result;
}

