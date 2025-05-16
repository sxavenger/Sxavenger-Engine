//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput : register(t0); //!< input texture

RWTexture2D<float4> gOutput : register(u1); //!< output texture

Texture3D<float4> gLUTTexture : register(t1); //!< LUT texture
SamplerState gLUTSampler      : register(s1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float4 input = gInput[index];
	input = saturate(input); //!< [0, 1]に収める

	float3 color = gLUTTexture.SampleLevel(gLUTSampler, input.rgb, 0);

	gOutput[index] = float4(color, input.a);
	
}
