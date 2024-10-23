//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Common
#include "SxavengerCommon.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
ConstantBuffer<ObjectMaterial> gMaterial : register(b0);

Texture2D<float4> gAlbed : register(t0);
SamplerState      gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {
	
	PSOutput output;
	
	float4 albed = gMaterial.GetColor(input.texcoord, gAlbed, gSampler) * gMaterial.color;
	output.color = albed;
	
	CheckDiscard(output.color);
	return output;
	
}