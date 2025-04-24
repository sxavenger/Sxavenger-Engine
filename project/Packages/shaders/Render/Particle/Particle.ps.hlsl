//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ParticlePass.hlsli"

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

	output.color.rgb = gParticles[input.instanceId].albedo.GetAlbedo(gSampler, input.texcoord);
	output.color.a   = gParticles[input.instanceId].transparent.GetTransparent(gSampler, input.texcoord);

	return output;
	
}
