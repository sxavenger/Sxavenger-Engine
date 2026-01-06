//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"
#include "../../Library/WeightedBlendedOIT.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gAccumulate : register(t0);
Texture2D<float> gRevealage   : register(t1);

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

	float4 accumulate = gAccumulate[index];
	float  revealage  = gRevealage[index];

	if (WeightedBlendedOIT::IsApproximatelyOpacity(revealage)) { //!< 完全不透明の場合 [ revealage == 1.0 ]
		return;
	}

	float3 transparency_color = WeightedBlendedOIT::CalculateTransparentColor(accumulate);
	float3 opacity_color      = gOutput[index].rgb;

	gOutput[index].rgb = lerp(opacity_color, transparency_color, 1.0f - revealage);
	gOutput[index].a   = 1.0f;
	
}
