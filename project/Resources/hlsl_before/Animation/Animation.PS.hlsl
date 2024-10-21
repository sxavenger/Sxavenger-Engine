//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Animation.hlsli"

//=========================================================================================
// Output
//=========================================================================================
struct PSOutput {
	float4 color : SV_Target0;
};

//=========================================================================================
// Buffer
//=========================================================================================
Texture2D<float4> gTexture : register(t0);
TextureCube<float4> gEnvironmentTexture : register(t1);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {
	
	PSOutput output;
	
	output.color = gTexture.Sample(gSampler, input.texcoord);
	
	float3 cameraToPosition = normalize(input.worldPos - gCamera.position.xyz);
	float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
	float4 enviornmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
	
	output.color.rgb = enviornmentColor.rgb;
	
	return output;
	
}