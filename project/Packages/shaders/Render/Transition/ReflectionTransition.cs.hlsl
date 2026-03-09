//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gReflection : register(t0);

RWTexture2D<float4> gOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;

	if (CheckOverTexture(index)) {
		return;
	}
	
	float4 reflection = gReflection[index];

	gOutput[index].rgb += reflection.rgb;
	gOutput[index].a   = reflection.a;
	
}
