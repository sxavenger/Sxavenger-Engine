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

	float4 color = (float4)0;
	color.rgb = gMaterials[input.instanceId].albedo.GetAlbedo(parameter);
	color.a   = gMaterials[input.instanceId].transparency.GetTransparency(parameter);

	float weight = WeightedBlendedOIT::CalculateWeight(color.a, input.position.z);

	output.SetColor(color, weight);
	return output;
	
}
