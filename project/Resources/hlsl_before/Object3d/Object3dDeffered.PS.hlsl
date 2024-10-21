//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Object3d.hlsli"
#include "../DefferedRendering.hlsli"
#include "../Camera.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
DefferedOutput main(VSOutput input) {
	
	DefferedOutput output;
	
	output.albed = gTexture.Sample(gSampler, input.texcoord);
	output.depth = Deffered::ToDepthColor(input.position);
	output.normal = Deffered::ToNormalColor(input.normal);
	output.worldPos = Deffered::ToWorldPosColor(input.worldPos);
	
	return output;
	
}