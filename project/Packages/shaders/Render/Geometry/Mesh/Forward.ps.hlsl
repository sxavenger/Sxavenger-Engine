//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryMesh.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
GeometryForwardOutput main(GeometryPSInput input) {

	GeometryForwardOutput output = (GeometryForwardOutput)0;

	MaterialLib::TextureSampler parameter;
	parameter.Set(input.texcoord, gSampler);

	output.color.rgb = gMaterials[input.instanceId].albedo.GetAlbedo(parameter);
	output.color.a   = gMaterials[input.instanceId].transparency.GetTransparency(parameter);

	return output;
	
}
