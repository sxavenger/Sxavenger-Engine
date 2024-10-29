//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Default.hlsli"
#include "DefaultMesh.hlsli"

static const float3 colors[] = {
	{ 0.62f, 0.56f, 1.0f },
	{ 0.49f, 0.71f, 0.86f },
	{ 1.0f, 0.54f, 0.64f },
	{ 0.51f, 0.71f, 0.64f },
	{ 1.0f, 0.57f, 1.0f },
	{ 0.88f, 1.0f, 0.59f },
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(128, 1, 1)]
[outputtopology("triangle")]
void main(
	uint groupThreadId : SV_GroupThreadID,
	uint groupId : SV_GroupID,
	in payload Payload payload,
	out vertices MSOutput verts[64],
	out indices uint3     polys[128]) {
	
	uint meshletIndex = payload.meshletIndices[groupId];
	
	if (meshletIndex >= gMeshInfo.meshletCount) {
		return;
	}
	
	Meshlet m = sMeshlets[meshletIndex];
	
	SetMeshOutputCounts(m.vertexCount, m.primitiveCount);
	
	if (groupThreadId < m.primitiveCount) {
		uint packedIndex = sPrimitives[m.primitiveOffset + groupThreadId];
		polys[groupThreadId] = UnpackPrimitiveIndex(packedIndex);
	}
	
	if (groupThreadId < m.vertexCount) {
		uint index = sIndices[m.vertexOffset + groupThreadId];
		MSInput input = sVertices[index];
		MSOutput output;
		
		output.position = mul(input.position, mul(gTransform.world, viewProj));
		output.worldPos = mul(input.position, gTransform.world);
		output.texcoord = input.texcoord;
		output.normal   = normalize(mul(input.normal, (float3x3)gTransform.world));
		output.color    = float4(colors[meshletIndex % 6], 1.0f);

		verts[groupThreadId] = output;
	}
}

