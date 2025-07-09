//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ParticlePass.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
ParticlePSInput main(PrimitiveVertex input, uint instanceId : SV_InstanceID) {

	ParticlePSInput output = (ParticlePSInput)0;

	float4x4 mat = gTransforms[instanceId].mat;
	// todo: matInvTranspose.

	if (gParticles[instanceId].isBillboard) {

		float4x4 view  = gCamera.view;
		float4x4 world = gCamera.world;
		
		//!< 制約: cameraにscale成分を入れない
		mat = float4x4(world[0], world[1], world[2], mat[3]);
		// todo: matInvTranspose.
	}
	
	output.
		position = mul(mul(input.position, mat), kViewProj);
	
	output.texcoord = input.texcoord;
	output.normal   = normalize(gTransforms[instanceId].TransformNormal(input.normal));

	output.instanceId = instanceId;
	
	return output;
	
}
