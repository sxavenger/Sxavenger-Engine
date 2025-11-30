//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SmaaCommon.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gEdge : register(t0);
RWTexture2D<float4> gOutput : register(u0); //!< BlendingWeight

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

	float2 pixcoord;
	float4 offset[3];
	SMAABlendingWeightCalculationVS(texcoord, pixcoord, offset);

	float4 weights = SMAABlendingWeightCalculationPS(texcoord, pixcoord, offset, gEdge, gAreaTexture, gSearchTexture, 0.0f);
	gOutput[index] = weights;
	
}
