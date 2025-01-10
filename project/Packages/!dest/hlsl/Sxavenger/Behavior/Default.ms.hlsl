//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DefaultMesh.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

ConstantBuffer<UVTransformationMatrix> gUVTransform : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(128, 1, 1)]
[outputtopology("triangle")]
void main(
	uint groupThreadId : SV_GroupThreadID,
	uint groupId : SV_GroupID,
	in payload Payload payload,
	out vertices PSInput  verts[_MAX_VERTICES],
	out indices uint3     polys[_MAX_INDICES]) {

	uint meshletIndex  = payload.meshletIndices[groupId];
	uint instanceIndex = payload.instanceIndices[groupId];
	
	if (meshletIndex >= gMeshInfo.meshletCount) {
		return;
	}
	
	Meshlet m = gMeshlets[meshletIndex];
	
	SetMeshOutputCounts(m.vertexCount, m.primitiveCount);
	
	if (groupThreadId < m.primitiveCount) {
		uint packedIndex     = gPrimitives[m.primitiveOffset + groupThreadId];
		polys[groupThreadId] = UnpackPrimitiveIndex(packedIndex);
	}
	
	if (groupThreadId < m.vertexCount) {
		uint index        = gIndices[m.vertexOffset + groupThreadId];
		VertexInput input = gVertices[index];
		
		PSInput output = (PSInput)0;
		
		output.position  = mul(gTransform[instanceIndex].Transform(input.position), viewProj);
		output.texcoord  = gUVTransform.Transform(input.texcoord);
		output.normal    = gTransform[instanceIndex].TransformNormal(input.normal);
		output.worldPos  = gTransform[instanceIndex].Transform(input.position).xyz;
		output.tangent   = gTransform[instanceIndex].TransformNormal(input.tangent);
		output.bitangent = gTransform[instanceIndex].TransformNormal(input.bitangent);

		verts[groupThreadId] = output;
	}
}
