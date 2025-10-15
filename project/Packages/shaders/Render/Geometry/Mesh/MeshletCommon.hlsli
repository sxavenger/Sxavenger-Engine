#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryMesh.hlsli"
#include "../../../Library/Meshlet.hlsli"

//=========================================================================================
// Visibility All buffers
//=========================================================================================

cbuffer Information : register(b10) {
	uint meshletCount;
}

StructuredBuffer<MeshVertex> gVertices : register(t10);
StructuredBuffer<uint> gIndices        : register(t11);
StructuredBuffer<Meshlet> gMeshlets    : register(t12);
StructuredBuffer<uint> gPrimitives     : register(t13);

