//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gPosition     : register(t0);
Texture2D<float4> gPrevPosition : register(t1);

ConstantBuffer<CameraComponent> gCamera : register(b0);

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

	float3 position     = gPosition.Load(uint3(index, 0)).xyz;
	float3 prevPosition = gPrevPosition.Load(uint3(index, 0)).xyz;

	float3 velocity = mul(float4(prevPosition, 1.0f), gCamera.view).xyz - mul(float4(position, 1.0f), gCamera.view).xyz;

	gVelocity[index] = float4(velocity, 1.0f);
	
}
