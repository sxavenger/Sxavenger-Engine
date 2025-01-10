//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Behavior.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t0);
Texture2D<float4> gNormal : register(t1);
SamplerState gSampler     : register(s0);

ConstantBuffer<MaterialColor> gColor : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
SystematicOutput main(PSInput input) {

	SystematicOutput output = (SystematicOutput)0;

	output.albedo   = ToAlbedo(gAlbedo.Sample(gSampler, input.texcoord) * gColor.color);
	output.position = ToPosition(input.worldPos);
	
	float3 texnormal = normalize(gNormal.Sample(gSampler, input.texcoord).rgb * 2.0f - 1.0f);
	float3x3 tbn = GetTangentSpaceMatrix(input.normal, input.tangent, input.bitangent);
	output.normal = ToNormal(mul(texnormal, tbn));

	return output;
	
}


