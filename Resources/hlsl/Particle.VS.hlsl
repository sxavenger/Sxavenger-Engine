//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Particle.hlsli"
#include "Camera.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// VSInput sturucture
////////////////////////////////////////////////////////////////////////////////////////////
struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

//=========================================================================================
// Buffer
//=========================================================================================
ConstantBuffer<Camera> gCamera : register(b0);
static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

struct ParticleData {
	float4x4 mat;
	float4   color;
};
StructuredBuffer<ParticleData> gParticleData : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	
	VSOutput output;
	
	output.position = mul(input.position, mul(gParticleData[instanceId].mat, viewProj));
	output.color    = gParticleData[instanceId].color;
	output.worldPos = mul(input.position, gParticleData[instanceId].mat).xyz;
	output.normal   = normalize(mul(input.normal, (float3x3)gParticleData[instanceId].mat));
	output.texcoord = input.texcoord;
	
	return output;
}