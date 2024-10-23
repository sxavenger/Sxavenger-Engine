//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "MeshCommon.hlsli"

//=========================================================================================
// Input
//=========================================================================================

struct MSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

//=========================================================================================
// buffers
//=========================================================================================

StructuredBuffer<MSInput> sVertices   : register(t0);
StructuredBuffer<uint> sIndices       : register(t1);
StructuredBuffer<Meshlet> sMeshlets   : register(t2);
StructuredBuffer<uint> sPrimitives    : register(t3);
StructuredBuffer<CullData> sCullData  : register(t4);

ConstantBuffer<MeshInfo> gMeshInfo              : register(b0);
ConstantBuffer<Camera> gCamera                  : register(b1);
ConstantBuffer<TransformationMatrix> gTransform : register(b2);
ConstantBuffer<Camera> gCullingCamera           : register(b3); //!< test buffer

static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);