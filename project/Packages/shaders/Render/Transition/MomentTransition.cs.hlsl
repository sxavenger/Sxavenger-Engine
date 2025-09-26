//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"
#include "../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<uint> gMoment    : register(u0);
RWTexture2D<float> gIndirect : register(u1);

Texture2D<float4> gVelocity : register(t0);

ConstantBuffer<CameraComponent> gCamera : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;

	if (CheckOverTexture(index)) {
		return;
	}

	

	
	
}
