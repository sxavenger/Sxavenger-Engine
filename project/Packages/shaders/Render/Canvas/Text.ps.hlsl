//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Text.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;

	float alpha = gFont.Sample(gSampler, input.texcoord);

	output.color = float4(input.color.rgb, input.color.a * alpha);
	
	return output;
	
}
