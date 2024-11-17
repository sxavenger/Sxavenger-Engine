//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Behavior.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t0);
SamplerState gSampler     : register(s0);

struct Color4 {
	float4 color;
};
ConstantBuffer<Color4> gColor : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
AdaptiveOutput main(PSInput input) {

	AdaptiveOutput output = (AdaptiveOutput)0;

	output.color = gAlbedo.Sample(gSampler, input.texcoord) * gColor.color;

	return output;
	
}