//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Common.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput : register(t0);

RWTexture2D<float4> gOutput : register(u0); //!< EdgeDetection

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;

	if (CheckOverDimension(index)) {
		return;
	}

	float2 texcoord = ((float2)index + 0.5f) / dimension;

	float4 offset[3];
	SMAAEdgeDetectionVS(texcoord, offset);

	float2 edges   = SMAALumaEdgeDetectionPS(texcoord, offset, gInput);
	gOutput[index] = float4(edges, 0.0f, 0.0f);
	
}
