//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyVisibilityCommon.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWStructuredBuffer<ReservoirLib:: Reservoir> gDestinationReservoir : register(u0);
StructuredBuffer<ReservoirLib:: Reservoir> gSourceReservoir : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return;
	}

	

}
