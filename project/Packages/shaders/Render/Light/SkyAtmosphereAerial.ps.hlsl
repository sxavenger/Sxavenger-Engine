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

SamplerState gAerialSampler : register(s0);

ConstantBuffer<SkyAtmosphereComponent> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;

	GBuffer::Surface surface;
	surface.FetchSurface(GBuffer::FetchArgument::Create(input.position.xy, dimension, gCamera.projInv, gCamera.world));

	float2 texcoord = (input.position.xy + 0.5f) / dimension;
	float2 viewport = dimension * 2.0f - 1.0f; //!< [-1, 1]の範囲で正規化されたスクリーン座標
	float3 direction = gCamera.GetDirection(float2(viewport.x, -viewport.y));

	float3 surface_position = surface.position;
	float3 camera_position  = gCamera.GetPosition();

	float distance = length(camera_position - surface_position);

	float4 color = gParameter.GetAerial(gAerialSampler, direction, distance);
	
	output.color.rgb = ACES::IDT_sRGB_AP1(color.rgb);
	output.color.a   = color.a;
	return output;
}
