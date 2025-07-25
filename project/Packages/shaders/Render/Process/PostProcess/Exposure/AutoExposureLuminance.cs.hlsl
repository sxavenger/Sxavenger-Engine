//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "AutoExposure.hlsli"
#include "../../../../Library/Math.hlsli"
#include "../../../../Library/ACES.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint CalculateBin(float3 color) {
	float luminance = dot(color, ACES::AP1_RGB2Y);
	
	if (luminance < kEpsilon) {
		return 0;
	}

	float logLuminance = saturate((log2(luminance) - gParameter.minLogLuminance) * kLogLuminanceRangeInv);
	return uint(logLuminance * 254.0 + 1.0);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float4 input = gInput[index];
	uint bin     = CalculateBin(input.rgb);

	InterlockedAdd(gHistogramShared[bin], 1); //!< increment luminance bin count
}
