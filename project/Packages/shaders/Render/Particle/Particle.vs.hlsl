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
		float4x4 world = gCamera.world;

		float3 scale    = float3(length(mat[0].xyz), length(mat[1].xyz), length(mat[2].xyz));
		float3 translate = mat[3].xyz;

		mat = float4x4(
			normalize(world[0]) * scale.x,
			normalize(world[1]) * scale.y,
			normalize(world[2]) * scale.z,
			float4(translate, 1.0f)
		);

	}
	
	output.position = mul(mul(input.position, mat), kViewProj);
	
	output.texcoord = input.texcoord;
	output.normal   = normalize(gTransforms[instanceId].TransformNormal(input.normal));

	output.instanceId = instanceId;
	
	return output;
	
}
