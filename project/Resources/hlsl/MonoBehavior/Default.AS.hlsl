//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DefaultMesh.hlsli"

groupshared Payload payload;

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_AMPLIFICATION_NUMTHREAD, 1, 1)]
void main(uint dispatchThreadId : SV_DispatchThreadID) {
	
	bool isVisible = false;
	
	if (dispatchThreadId < gMeshInfo.meshletCount) {
		isVisible = IsVisible(planes, gCullData[dispatchThreadId], gTransform.world, gCamera.position);
	}
	
	if (isVisible) {
		uint index = WavePrefixCountBits(isVisible);
		payload.meshletIndices[index] = dispatchThreadId;
	}
	
	uint visibleCount = WaveActiveCountBits(isVisible);
	DispatchMesh(visibleCount, 1, 1, payload);
}