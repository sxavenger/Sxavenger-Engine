#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../MeshCommon.hlsli"
#include "Behavior.hlsli"

//=========================================================================================
// Visibility All buffers
//=========================================================================================

StructuredBuffer<VertexInput> gVertices : register(t10);
StructuredBuffer<uint> gIndices         : register(t11);
StructuredBuffer<Meshlet> gMeshlets     : register(t12);
StructuredBuffer<uint> gPrimitives      : register(t13);
StructuredBuffer<CullData> gCullData    : register(t14);

ConstantBuffer<MeshInfo> gMeshInfo : register(b11);

// ConstantBuffer<Camera> gCamera : register(b10); //!< Behavior.hlsli
static const FrustumPlanes planes = CreateFrustumPlanes(gCamera.viewMatrix, gCamera.projMatrix);

StructuredBuffer<TransformationMatrix> gTransform : register(t15); //!< instance前提

