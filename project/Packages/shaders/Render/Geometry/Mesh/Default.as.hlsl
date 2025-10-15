//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "MeshletCommon.hlsli"

//=========================================================================================
// group shared
//=========================================================================================

groupshared Payload payload;

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_AMPLIFICATION_NUMTHREAD, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	//* dispatchThreadId.yはinstaceのindex
	
	bool isVisible = (dispatchThreadId.x < meshletCount);
	
	if (isVisible) {
		uint index = WavePrefixCountBits(isVisible);
		payload.meshletIndices[index]  = dispatchThreadId.x;
		payload.instanceIndices[index] = dispatchThreadId.y;
	}
	
	uint visibleCount = WaveActiveCountBits(isVisible);
	DispatchMesh(visibleCount, 1, 1, payload);
}
