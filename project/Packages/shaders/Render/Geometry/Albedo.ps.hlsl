//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPass.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t0);
SamplerState gSampler     : register(s0);

ConstantBuffer<MaterialComponent> gMaterial : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryForwardOutput main(GeometryPSInput input) {

	GeometryForwardOutput output = (GeometryForwardOutput)0;

	output.color.rgb = gMaterial.albedo.GetAlbedo(input.texcoord, gSampler);
	output.color.a   = gMaterial.transparency.GetTrnasparency(input.texcoord, gSampler);

	return output;
	
}
