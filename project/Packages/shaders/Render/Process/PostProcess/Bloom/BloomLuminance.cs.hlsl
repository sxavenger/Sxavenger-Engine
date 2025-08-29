//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Bloom.hlsli"
#include "../../../../Library/ACES.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput    : register(t0);
RWTexture2D<float4> gOutput : register(u0);

SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float CalculateLuminance(float3 color) {
	return max(dot(color, ACES::AP1_RGB2Y) - gParameter.threshold, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispathThreadId : SV_DispatchThreadID) {

	uint2 index = dispathThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return;
	}

	float4 color = gInput[index];

	float luminance = CalculateLuminance(color.rgb);
	gOutput[index]  = float4(luminance, luminance, luminance, color.a);
	
}
