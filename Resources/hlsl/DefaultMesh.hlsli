//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Camera.hlsli"

//=========================================================================================
// Configs
//=========================================================================================

#define AS_GROUP_SIZE 32 //!< test value

//=========================================================================================
// Input
//=========================================================================================

struct MSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

struct Payload {
	uint meshletIndices[AS_GROUP_SIZE];
};

//=========================================================================================
// sutructures
//=========================================================================================

struct Meshlet {
	uint vertexCount;
	uint vertexOffset;
	uint primitiveCount;
	uint primitiveOffset;
};

struct MeshInfo {
	uint meshletCount;
};

struct CullData {
	float4 boundingSphere;
	uint normalCone;
	float apexOffset;
};

struct TransformationMatrix {
	float4x4 world;
	float4x4 worldInverseTranspose;
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

//=========================================================================================
// methods
//=========================================================================================

uint3 UnpackPrimitiveIndex(uint packedIndex) {
	return uint3(
		packedIndex & 0x3FF,
		(packedIndex >> 10) & 0x3FF,
		(packedIndex >> 20) & 0x3FF
	);
}