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
[earlydepthstencil]
GeometryForwardOutput main(GeometryPSInput input) {

	GeometryForwardOutput output = (GeometryForwardOutput)0;

	Material::TextureParameter parameter;
	parameter.Set(input.texcoord, gSampler);

	output.color.rgb = gMaterial.albedo.GetAlbedo(parameter);
	output.color.a   = gMaterial.transparency.GetTransparency(parameter);

	return output;
	
}
