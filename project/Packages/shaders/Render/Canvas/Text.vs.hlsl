//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Text.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSInput main(VSInput input) {
	
	PSInput output = (PSInput)0;

	float3 transformed = gTransform.TransformNormalize(input.position.xy);
	
	output.position.xy = transformed.xy / float2(size) * 2.0f - 1.0f;
	output.position.z  = transformed.z;
	output.position.w  = 1.0f;

	output.texcoord = input.texcoord;
	
	//* to screen
	output.position.y *= -1.0f;
	
	output.color = input.color;
	
	return output;
}
