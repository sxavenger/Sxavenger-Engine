#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../MeshCommon.hlsli"
#include "MonoBehavior.hlsli"

//=========================================================================================
// Visibility All buffers
//=========================================================================================

StructuredBuffer<VertexInput> gVertices : register(t10);
StructuredBuffer<uint> gIndices         : register(t11);
StructuredBuffer<Meshlet> gMeshlets     : register(t12);
StructuredBuffer<uint> gPrimitives      : register(t13);
StructuredBuffer<CullData> gCullData    : register(t14);

// ConstantBuffer<Camera> gCamera : register(b10); //!< MonoBehavior.hlsli
static const FrustumPlanes planes = CreateFrustumPlanes(gCamera.viewMatrix, gCamera.projMatrix);
ConstantBuffer<MeshInfo> gMeshInfo              : register(b11);
ConstantBuffer<TransformationMatrix> gTransform : register(b12);


