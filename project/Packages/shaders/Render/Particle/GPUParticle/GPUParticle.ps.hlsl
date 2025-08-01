//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GPUParticlePass.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
GeometryForwardOutput main(ParticlePSInput input) {

	GeometryForwardOutput output = (GeometryForwardOutput)0;

	GPUParticleComponent particle = gParticles[input.instanceId];

	output.color.rgb = gConfig.albedo.GetAlbedo(gSampler, input.texcoord) * particle.albedo;
	output.color.a   = gConfig.transparent.GetTransparent(gSampler, input.texcoord) * particle.transparent;

	return output;
	
}
