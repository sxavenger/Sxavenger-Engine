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

	float2 texcoord = (input.position.xy + 0.5f) / dimension;
	float2 viewport = dimension * 2.0f - 1.0f; //!< [-1, 1]の範囲で正規化されたスクリーン座標
	float3 direction = gCamera.GetDirection(float2(viewport.x, -viewport.y));

	float4 color = gParameter.GetEnvironment(gSampler, direction);
	
	output.color.rgb = ACES::IDT_sRGB_AP1(color.rgb);
	output.color.a   = 1.0f;
	return output;
}
