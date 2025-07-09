//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "AutoExposure.hlsli"
#include "../../../../Library/Math.hlsli"

//=========================================================================================
// const variables
//=========================================================================================

static const float3 kLuminance = float3(0.2125, 0.7154, 0.0721);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint CalculateBin(float3 color) {
	float luminance = dot(color, kLuminance);
	
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
