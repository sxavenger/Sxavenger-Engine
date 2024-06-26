//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Particle.hlsli"
#include "DefferedRendering.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
DefferedOutput main(VSOutput input) {

	DefferedOutput output;
	
	output.albed    = input.color;
	output.depth    = Deffered::ToDepthColor(input.position);
	output.normal   = Deffered::ToNormalColor(input.normal);
	output.worldPos = Deffered::ToWorldPosColor(input.worldPos);
	
	return output;
}