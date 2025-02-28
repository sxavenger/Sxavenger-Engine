//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ColliderPrimitive.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {

	VSOutput output;

	output.position = mul(input.position, mul(gParameter.mat, kViewProj));
	output.color    = gParameter.color;

	return output;
}
