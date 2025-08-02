//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GPUParticlePass.hlsli"
#include "../../../Library/ACES.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

SamplerState gSampler : register(s0);

// test
Texture2D<float4> gDirect : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
GeometryForwardOutput main(ParticlePSInput input) {

	GeometryForwardOutput output = (GeometryForwardOutput)0;

	GPUParticleComponent particle = gParticles[input.instanceId];

	float3 direct = gDirect.Sample(gSampler, input.projection).rgb;

	output.color.rgb = gConfig.albedo.GetAlbedo(gSampler, input.texcoord) * particle.albedo * direct;
	output.color.a   = gConfig.transparent.GetTransparent(gSampler, input.texcoord) * particle.transparent;

	return output;
	
}
