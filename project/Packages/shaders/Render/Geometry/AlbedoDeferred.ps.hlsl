//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPass.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

Texture2D<float4> gAlbedo : register(t0);
SamplerState gSampler     : register(s0);

ConstantBuffer<MaterialComponent> gMaterial : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryDeferredOutput main(GeometryPSInput input) {
	
	GeometryDeferredOutput output = (GeometryDeferredOutput)0;

	output.SetAlbedo(gMaterial.albedo.GetAlbedo(input.texcoord, gSampler));
	output.SetNormal(gMaterial.normal.GetNormal(input.normal, input.texcoord, gSampler));
	output.SetPosition(input.worldPos);
	
	return output;
}
