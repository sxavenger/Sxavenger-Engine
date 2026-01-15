//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryPrimitive.hlsli"

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
	color.rgb = float3(1.0f, 1.0f, 1.0f);
	color.a   = 1.0f;

	float weight = WeightedBlendedOIT::CalculateWeight(color.a, input.position.z);

	output.SetColor(color, weight);
	return output;
}
