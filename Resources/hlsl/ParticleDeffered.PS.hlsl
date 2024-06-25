//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Particle.hlsli"
#include "DefferedRendering.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
DefferedOutput main(VSOutput input) {

	DefferedOutput output;
	
	output.albed    = input.color;
	output.depth    = Deffered::ToDepthColor(input.position);
	output.normal   = Deffered::ToNormalColor(input.normal);
	output.worldPos = float4(input.worldPos, 1.0f);
	
	return output;
}