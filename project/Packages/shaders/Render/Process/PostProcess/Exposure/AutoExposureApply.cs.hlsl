//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "AutoExposure.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float4 input   = gInput[index];
	float exposure = 1.0f / (gAverageLuminance[0] * exp2(-gParameter.compensation));

	float4 output = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.rgb = input.rgb * exposure;
	output.a   = input.a;

	gOutput[index].rgb = lerp(input.rgb, output.rgb, blendWeight);
	gOutput[index].a = input.a;

}
