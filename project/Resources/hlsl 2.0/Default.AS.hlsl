//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DefaultMesh.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

groupshared Payload payload;

//=========================================================================================
// FrustumPlanes
//=========================================================================================

struct FrustumPlanes {
	float4 planes[6];
};

namespace FrustumPlaneType {
	static const uint LEFT   = 0,
	                  RIGHT  = 1,
	                  BOTTOM = 2,
	                  TOP    = 3,
	                  NEAR   = 4,
	                  FAR    = 5;
}

//=========================================================================================
// methods
//=========================================================================================

FrustumPlanes CalculateFrustumPlanes(float4x4 view, float4x4 proj) {
	
	FrustumPlanes result;
	
	float4x4 clipMatrix = transpose(mul(view, proj));
	
	result.planes[FrustumPlaneType::LEFT]   = clipMatrix[3] + clipMatrix[0];
	result.planes[FrustumPlaneType::RIGHT]  = clipMatrix[3] - clipMatrix[0];
	result.planes[FrustumPlaneType::BOTTOM] = clipMatrix[3] + clipMatrix[1];
	result.planes[FrustumPlaneType::TOP]    = clipMatrix[3] - clipMatrix[1];
	result.planes[FrustumPlaneType::NEAR]   = clipMatrix[2];
	result.planes[FrustumPlaneType::FAR]    = clipMatrix[3] - clipMatrix[2];
	
	for (uint i = 0; i < 6; ++i) {
		result.planes[i] /= length(result.planes[i].xyz);
	}
	
	return result;
	
}

//=========================================================================================
// static variables
//=========================================================================================

static const FrustumPlanes planes = CalculateFrustumPlanes(gCullingCamera.viewMatrix, gCullingCamera.projMatrix);

//=========================================================================================
// culling methods
//=========================================================================================

bool IsVisible(CullData c, float4x4 world) {
	
	float3 scale = float3(length(world[0]), length(world[1]), length(world[2]));
	float maxScale = max(scale.x, max(scale.y, scale.z));
	
	float4 center = mul(float4(c.boundingSphere.xyz, 1.0f), world);
	float radius = c.boundingSphere.w * maxScale;
	
	// frustum culling
	for (uint i = 0; i < 6; ++i) {
		if (dot(center.xyz, planes.planes[i].xyz) + planes.planes[i].w <= -radius) {
			return false;
		}
	}
	
	// todo: other culling
	
	
	
	return true;

}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(AS_GROUP_SIZE, 1, 1)]
void main(uint groupThreadId : SV_GroupThreadID, uint groupId : SV_GroupID, uint dispatchThreadId : SV_DispatchThreadID) {
	
	bool isVisible = false;
	
	if (dispatchThreadId < gMeshInfo.meshletCount) {
		isVisible = IsVisible(sCullData[dispatchThreadId], gTransform.world);
	}
	
	if (isVisible) {
		uint index = WavePrefixCountBits(isVisible);
		payload.meshletIndices[index] = dispatchThreadId;
	}
	
	uint visibleCount = WaveActiveCountBits(isVisible);
	DispatchMesh(visibleCount, 1, 1, payload);
}