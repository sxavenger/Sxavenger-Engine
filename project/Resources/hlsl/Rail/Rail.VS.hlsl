//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Rail.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {

	VSOutput output;

	output.positionLeft  = input.positionLeft;
	output.positionRight = input.positionRight;
	output.color         = input.color;

	return output;
	
}
