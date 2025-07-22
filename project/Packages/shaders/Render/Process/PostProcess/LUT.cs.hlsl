//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Process.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gOutput : register(u0); //!< output texture
Texture2D<float4> gInput    : register(t0); //!< input texture

Texture3D<float3> gLUTTexture : register(t1); //!< LUT texture
SamplerState gLUTSampler      : register(s0); //!< sampler state for LUT texture

struct Parameter {
	float intensity;
};
ConstantBuffer<Parameter> gParameter : register(b0);

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
	float3 grading = gLUTTexture.SampleLevel(gLUTSampler, saturate(color.rgb), 0); //!< sample LUT texture

	// todo: 0.0 ~ 1.0 範囲外の対応

	gOutput[index].rgb = lerp(color.rgb, grading, gParameter.intensity);
	gOutput[index].a   = color.a;
	
}
