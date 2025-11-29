//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* library
#include "../../Library/BRDF.hlsli"
#include "../../Library/ACES.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

SamplerState gSampler : register(s0);

TextureCube<float4> gEnvironment : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;

	float2 d         = (input.position.xy + 0.5f) / size.xy * 2.0f - 1.0f;
	float3 target    = mul(float4(d.x, -d.y, 1.0f, 1.0f), gCamera.projInv).xyz;
	float3 direction = mul(float4(target, 0.0f), gCamera.world).xyz;

	float4 color = gEnvironment.SampleLevel(gSampler, direction, 0);
	
	output.color.rgb = ACES::IDT_sRGB_AP1(color.rgb);
	output.color.a   = 1.0f;
	return output;
}
