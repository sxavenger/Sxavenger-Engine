//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput    : register(t0); //!< input texture
RWTexture2D<float4> gOutput : register(u1); //!< output texture

Texture3D<float4> gLUTTexture : register(t1); //!< LUT texture
SamplerState gLUTSampler      : register(s0); //!< sampler state for LUT texture

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float4 color = gInput[index];

	// LUT
	color.rgb = saturate(color.rgb); //!< clamp to [0, 1]
	color.rgb = gLUTTexture.SampleLevel(gLUTSampler, color.rgb, 0).rgb; //!< sample LUT texture

	gOutput[index] = color;
	
}
