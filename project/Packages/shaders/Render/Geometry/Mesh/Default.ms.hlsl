//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "MeshletCommon.hlsli"

//=========================================================================================
// buffers
//=========================================================================================


////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(128, 1, 1)]
[outputtopology("triangle")]
void main(
	uint groupThreadId : SV_GroupThreadID,
	uint groupId : SV_GroupID,
	in payload Payload payload,
	out vertices GeometryPSInput vert[_MAX_VERTICES],
	out indices uint3 prim[_MAX_INDICES]) {

	uint meshletIndex  = payload.meshletIndices[groupId];
	uint instanceIndex = payload.instanceIndices[groupId];

	if (meshletIndex >= meshletCount) {
		return;
	}

	Meshlet meshlet = gMeshlets[meshletIndex];

	SetMeshOutputCounts(meshlet.vertexCount, meshlet.triangleCount);

	if (groupThreadId < meshlet.triangleCount) {
		uint packedIndex    = gPrimitives[meshlet.triangleOffset + groupThreadId];
		prim[groupThreadId] = UnpackPrimitiveIndex(packedIndex);
	}

	if (groupThreadId < meshlet.vertexCount) {
		uint index       = gIndices[meshlet.vertexOffset + groupThreadId];
		MeshVertex input = gVertices[index];

		GeometryPSInput output = (GeometryPSInput)0;

		output.position = mul(gTransforms[instanceIndex].Transform(input.position), kViewProj);
		output.worldPos = gTransforms[instanceIndex].Transform(input.position).xyz;
		
		output.texcoord = gMaterials[instanceIndex].transform.Transformation(input.texcoord);
		
		output.normal    = normalize(gTransforms[instanceIndex].TransformNormal(input.normal));
		output.tangent   = normalize(gTransforms[instanceIndex].TransformNormal(input.tangent));
		output.bitangent = normalize(gTransforms[instanceIndex].TransformNormal(input.bitangent));

		output.instanceId = instanceIndex;

		vert[groupThreadId] = output;
		
	}
}
