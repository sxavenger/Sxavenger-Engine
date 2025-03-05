//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPass.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t0);
SamplerState gSampler     : register(s0);

StructuredBuffer<MaterialComponent> gMaterials : register(t1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
GeometryForwardOutput main(GeometryPSInput input) {

	GeometryForwardOutput output = (GeometryForwardOutput)0;

	Material::TextureParameter parameter;
	parameter.Set(input.texcoord, gSampler);

	output.color.rgb = gMaterials[input.instanceId].albedo.GetAlbedo(parameter);
	output.color.a   = gMaterials[input.instanceId].transparency.GetTransparency(parameter);

	return output;
	
}
