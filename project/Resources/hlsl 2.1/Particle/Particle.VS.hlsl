//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Particle.hlsli"
#include "../Camera.hlsli"
#include "../Transform.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

ConstantBuffer<Camera> gCamera : register(b0);
static const float4x4 kViewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

StructuredBuffer<TransformationMatrix> gTransform : register(t0);

struct ParticleInfo {
	float4 color;
};
StructuredBuffer<ParticleInfo> gInfo : register(t1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {

	VSOutput output = (VSOutput)0;

	output.position = mul(input.position, mul(gTransform[instanceId].world, kViewProj));
	output.texcoord = input.texcoord;
	output.color    = gInfo[instanceId].color;

	return output;

}