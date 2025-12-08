//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "MeshletCommon.hlsli"

//=========================================================================================
// group shared
//=========================================================================================

groupshared Payload payload;

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

bool VisibleMeshlet(Bounds bounds, TransformComponent transform) {

	//* frustum culling

	Frustum frustum = Frustum::Create(gCullCamera.GetViewProj());

	float3 center = transform.Transform(float4(bounds.center, 1.0f)).xyz;
	float radius  = bounds.radius * transform.GetMaxScale();

	[unroll]
	for (uint i = 0; i < 6; ++i) {
		if ((dot(frustum.planes[i].xyz, center) + frustum.planes[i].w) <= -radius) {
			return false;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_AMPLIFICATION_NUMTHREAD, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	//* dispatchThreadId.yはinstaceのindex
	
	bool isVisible = false;
	
	if (dispatchThreadId.x < meshletCount) {
		isVisible = VisibleMeshlet(gBounds[dispatchThreadId.x], gTransforms[dispatchThreadId.y]);
	}
	
	if (isVisible) {
		uint index = WavePrefixCountBits(isVisible);
		payload.meshletIndices[index]  = dispatchThreadId.x;
		payload.instanceIndices[index] = dispatchThreadId.y;
	}
	
	uint visibleCount = WaveActiveCountBits(isVisible);
	DispatchMesh(visibleCount, 1, 1, payload);
}
