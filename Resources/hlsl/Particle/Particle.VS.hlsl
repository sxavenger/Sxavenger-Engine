//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Particle.hlsli"
#include "../Camera.hlsli"
#include "../Matrix.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// VSInput sturucture
////////////////////////////////////////////////////////////////////////////////////////////
struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

//=========================================================================================
// Buffer
//=========================================================================================
ConstantBuffer<Camera> gCamera : register(b0);
static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

StructuredBuffer<Particle> gParticle : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	
	VSOutput output;
	
	Particle particle = gParticle[instanceId];
	
	float4x4 worldMatrix = kIdentity;
	worldMatrix = mul(worldMatrix, MakeAffine(particle.scale, (float3)0, particle.translate));
	//!< TODO: billBoradの適応をする
	
	output.position = mul(input.position, mul(worldMatrix, viewProj));
	output.color    = particle.color;
	output.worldPos = mul(input.position, worldMatrix).xyz;
	output.normal   = normalize(mul(input.normal, (float3x3)worldMatrix));
	output.texcoord = input.texcoord;
	
	return output;
}