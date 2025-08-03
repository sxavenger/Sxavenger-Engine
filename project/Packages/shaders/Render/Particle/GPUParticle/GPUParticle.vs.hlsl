//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GPUParticlePass.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
ParticlePSInput main(PrimitiveVertex input, uint instanceId : SV_InstanceID) {

	ParticlePSInput output = (ParticlePSInput)0;

	GPUParticleComponent particle = gParticles[instanceId];

	float4x4 mat = (float4x4)0;

	if (gConfig.isBillboard) {
		
		float4x4 camera = gCamera.world;

		mat = float4x4(
			normalize(camera[0]),
			normalize(camera[1]),
			normalize(camera[2]),
			float4(particle.translate, 1.0f)
		);

		// TODO: Position Billboard
		// TODO: Coordinate Billboard

	} else {
		// todo: billboardではない場合の処理
	}

	mat[0] *= particle.scale.x;
	mat[1] *= particle.scale.y;
	mat[2] *= particle.scale.z;

	output.position = mul(input.position, mul(mat, kViewProj));
	output.normal   = normalize(mul(input.normal, (float3x3)mat)); // todo: inverse transpose
	output.texcoord = input.texcoord;

	float4 proj = mul(float4(input.position.xyz * 0.1f, 1.0f), mul(mat, kViewProj));
	output.projection = proj.xy / proj.w;
	output.projection = (output.projection + 1.0f) * 0.5f;
	output.projection.y = 1.0f - output.projection.y;

	output.instanceId = instanceId;

	return output;
}
