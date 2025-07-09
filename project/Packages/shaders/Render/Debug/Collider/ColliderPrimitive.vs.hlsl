//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ColliderPrimitive.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {

	VSOutput output;

	output.position = mul(input.position, mul(gParameter[instanceId].mat, kViewProj));
	output.color    = gParameter[instanceId].color;

	return output;
}
