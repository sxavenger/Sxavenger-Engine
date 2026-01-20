//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* library
#include "../../Library/BRDF.hlsli"
#include "../../Library/ACES.hlsli"

//* component
#include "../../Component/SkyAtmosphereComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

SamplerState gSampler : register(s0);

ConstantBuffer<SkyAtmosphereComponent> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;

	float2 uv = (input.position.xy + 0.5f) / size.xy * 2.0f - 1.0f;
	float3 direction = gCamera.GetDirection(float2(uv.x, -uv.y));

	float4 color = gParameter.GetEnvironment(gSampler, direction);
	
	output.color.rgb = ACES::IDT_sRGB_AP1(color.rgb);
	output.color.a   = 1.0f;
	return output;
}
