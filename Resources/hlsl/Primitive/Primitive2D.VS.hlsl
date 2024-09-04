//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Primitive.hlsli"
#include "../Camera.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	output.position = input.position;
	output.texcoord = input.texcoord;
	output.color    = input.color;
	
	return output;
	
}