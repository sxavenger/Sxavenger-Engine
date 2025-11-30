//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SmaaCommon.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput       : register(t0);
Texture2D<float4> gBlendWeight : register(t1);

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

	float2 texcoord = (float2)index / (float2)size;

	float4 offset;
	SMAANeighborhoodBlendingVS(texcoord, offset);

	float4 color   = SMAANeighborhoodBlendingPS(texcoord, offset, gInput, gBlendWeight);
	gOutput[index] = color;
	
}
