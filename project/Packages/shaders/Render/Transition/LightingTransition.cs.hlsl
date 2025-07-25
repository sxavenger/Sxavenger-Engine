//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gDirect   : register(t0);
Texture2D<float4> gIndirect : register(t1);

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
	
	float4 direct   = gDirect[index];
	float4 indirect = gIndirect[index];

	float4 output = direct + indirect;
	output.a = 1.0f;

	gOutput[index] = output;
	
}
