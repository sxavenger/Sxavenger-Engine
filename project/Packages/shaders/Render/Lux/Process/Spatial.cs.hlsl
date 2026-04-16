//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* Lux
#include "ProcessCommon.hlsli"

//* component
#include "../../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	//!< Atlas Dimension での Dispatch 想定

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return;
	}

	GBuffer::Surface surface;
	if (!surface.FetchSurface(GBuffer::FetchArgument::Create(pixel * setting.downscale, dimension, gCamera.projInv, gCamera.world))) {
		return; //!< surfaceが存在しない場合
	}
	
}
