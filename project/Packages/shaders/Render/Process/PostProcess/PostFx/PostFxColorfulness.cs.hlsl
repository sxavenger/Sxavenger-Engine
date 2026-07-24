//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostFx.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;
	
	if (CheckOverDimension(pixel)) {
		return; //!< texture size over
	}

	float3 color       = gInput[pixel].rgb;
	float transparency = gInput[pixel].a;

	float3 colorfulness = gParameter.GetColorfulness(color);
	
	gOutput[pixel].rgb = lerp(color, colorfulness, blendWeight);
	gOutput[pixel].a   = transparency;

}
