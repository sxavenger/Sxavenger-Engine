//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gPosition : register(t0);

ConstantBuffer<CameraComponent> gCurrentCamera : register(b0);
ConstantBuffer<CameraComponent> gPrevCamera    : register(b1);

RWTexture2D<float4> gVelocity : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	uint2 index = dispatchThreadId.xy;

	if (CheckOverTexture(index)) {
		return;
	}

	float3 position = gPosition.Load(uint3(index, 0)).xyz;

	float2 current = gCurrentCamera.CalculateNDCPosition(position);
	float2 prev    = gPrevCamera.CalculateNDCPosition(position);

	float2 delta = current - prev;
	delta.y *= -1.0f; //!< y軸反転

	gVelocity[index] = float4(delta, 0.0f, 1.0f);
	
}
