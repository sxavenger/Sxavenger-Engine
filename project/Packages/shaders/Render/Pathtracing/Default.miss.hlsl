//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PathtracingCommon.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// miss main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_MISS void mainMiss(inout Payload payload) {

	payload.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	payload.depth = 1.0f;
	
}
