//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Behavior.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t0);
SamplerState gSampler     : register(s0);

ConstantBuffer<PBRMaterial> gMaterial : register(b0);

struct Color4 {
	float4 color;
};
ConstantBuffer<Color4> gColor : register(b1);


////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
SystematicOutput main(PSInput input) {

	SystematicOutput output = (SystematicOutput)0;

	output.albedo   = ToAlbedo(gAlbedo.Sample(gSampler, input.texcoord) * gColor.color);
	output.normal   = ToNormal(input.normal);
	output.position = ToPosition(input.worldPos);

	PBRMaterial material = gMaterial;
	output.material = ToMaterial(material);

	return output;
}

