//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Smoke.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t0);
SamplerState gSampler     : register(s0);

StructuredBuffer<MaterialColor> gColor : register(t1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
AdaptiveOutput main(PSInput input) {

	AdaptiveOutput output = (AdaptiveOutput)0;

	output.color = gAlbedo.Sample(gSampler, input.texcoord) * gColor[input.instanceId].color;
	//output.color = gAlbedo.Sample(gSampler, input.texcoord) * float4(1.0f, 1.0f, 1.0f, 0.4f);

	return output;
	
}