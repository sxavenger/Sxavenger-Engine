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
	
	//* parameter
	const float3 sampleLightDirection = float3(0.0f, -1.0f, 0.0f); //!< 下向きのsampleライト
	
	PSOutput output;
	
	float4 albed = gMaterial.GetAlbed(input.texcoord, gAlbed, gSampler) * gMaterial.color;
	
	output.color.rgb = HalfLambertReflection(input.normal, -sampleLightDirection) * albed.rgb;
	output.color.a   = albed.a;
	
	CheckPSOutput(output);
	return output;
	
}