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
GeometryForwardTransparentOutput main(GeometryPSInput input) {
	
	GeometryForwardTransparentOutput output = (GeometryForwardTransparentOutput)0;

	MaterialLib::TextureSampler parameter;
	parameter.Set(input.texcoord, gSampler);

	float3 albedo      = gMaterials[input.instanceId].albedo.GetAlbedo(parameter);
	float transparency = gMaterials[input.instanceId].transparency.GetTransparency(parameter);

	float weight = WeightedBlendedOIT::CalculateWeight(transparency, input.position.z, gCamera.near, gCamera.far);

	output.SetColor(albedo, transparency, weight);
	return output;

}
