//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Sprite.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSInput main(VSInput input) {
	PSInput output = (PSInput)0;
	
	output.position.xy = gTransformation2d.Transform(input.position.xy) / float2(gParameter.size) * 2.0f - 1.0f;
	output.position.z  = input.position.z;
	output.position.w  = 1.0f;

	output.texcoord = mul(float4(input.texcoord, 0.0f, 1.0f), gTransformationUV.mat).xy;
	
	//* to screen
	output.position.y *= -1.0f;
	
	output.color = input.color;
	
	return output;
}
