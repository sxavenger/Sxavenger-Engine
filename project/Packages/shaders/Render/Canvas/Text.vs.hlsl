//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Text.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSInput main(VSInput input) {
	
	PSInput output = (PSInput)0;
	
	output.position.xy = gTransform.Transform(input.position.xy) / float2(size) * 2.0f - 1.0f;
	output.position.z  = input.position.z;
	output.position.w  = 1.0f;

	output.texcoord = input.texcoord;
	
	//* to screen
	output.position.y *= -1.0f;
	
	output.color = input.color;
	
	return output;
}
