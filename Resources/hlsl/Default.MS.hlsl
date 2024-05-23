//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Default.hlsli"
#include "Camera.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Mesh structure
////////////////////////////////////////////////////////////////////////////////////////////

struct MSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

struct Meshlet {
	uint vertexOffset;
	uint vertexCount;
	uint primitiveOffset;
	uint primitiveCount;
};

//-----------------------------------------------------------------------------------------
// Buffers
//-----------------------------------------------------------------------------------------
StructuredBuffer<MSInput> sVertices : register(t0);
StructuredBuffer<uint>    sIndices : register(t1);
StructuredBuffer<Meshlet> sMeshlets : register(t2);
StructuredBuffer<uint>    sPrimitives : register(t3);

//=========================================================================================
// TransformationMatrix structure buffer
//=========================================================================================
struct TransformationMatrix {
	float4x4 world;
	float4x4 worldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransform : register(b0);

// camera
ConstantBuffer<Camera> gCamera : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint3 UnpackPrimitiveIndex(uint packedIndex) {
	return uint3(
		packedIndex & 0x3FF,
		(packedIndex >> 10) & 0x3FF,
		(packedIndex >> 20) & 0x3FF
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(128, 1, 1)]
[outputtopology("triangle")]
void main(
	uint groupThreadId : SV_GroupThreadID,
	uint groupId : SV_GroupID,
	out vertices MSOutput verts[64],
	out indices uint3     polys[126]) {
	
	Meshlet m = sMeshlets[groupId];
	
	SetMeshOutputCounts(m.vertexCount, m.primitiveCount);
	
	if (groupThreadId < m.primitiveCount) {
		uint packedIndex = sPrimitives[m.primitiveOffset + groupThreadId];
		polys[groupThreadId] = UnpackPrimitiveIndex(packedIndex);

	}
	
	if (groupThreadId < m.vertexCount) {
		uint index = sIndices[m.vertexOffset + groupThreadId];
		MSInput input = sVertices[index];
		MSOutput output;
		
		output.position = mul(input.position, mul(gTransform.world, gCamera.viewProj));
		output.worldPos = mul(input.position, gTransform.world);
		output.texcoord = input.texcoord;
		output.normal   = normalize(mul(input.normal, (float3x3)gTransform.world));

		verts[groupThreadId] = output;
	}
}

