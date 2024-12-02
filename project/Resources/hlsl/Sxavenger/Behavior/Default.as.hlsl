//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DefaultMesh.hlsli"

groupshared Payload payload;

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_AMPLIFICATION_NUMTHREAD, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	//* dispatchThreadId.yはinstaceのindex
	
	bool isVisible = false;
	
	if (dispatchThreadId.x < gMeshInfo.meshletCount) {
		isVisible = IsVisible(planes, gCullData[dispatchThreadId.x], gTransform[dispatchThreadId.y].mat, gCamera.position);
	}
	
	if (isVisible) {
		uint index = WavePrefixCountBits(isVisible);
		payload.meshletIndices[index]  = dispatchThreadId.x;
		payload.instanceIndices[index] = dispatchThreadId.y;
	}
	
	uint visibleCount = WaveActiveCountBits(isVisible);
	DispatchMesh(visibleCount, 1, 1, payload);
}