//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "AutoExposure.hlsli"

//=========================================================================================
// constant
//=========================================================================================

static const uint kPixelNum = gConfig.size.x * gConfig.size.y;

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_GROUP_SIZE, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	InterlockedAdd(gHistogram[dispatchThreadId.x], gHistogramShared[dispatchThreadId.x]);

	GroupMemoryBarrierWithGroupSync();

	uint countBin = gHistogram[dispatchThreadId.x];
	gHistogramShared[dispatchThreadId.x] = countBin * dispatchThreadId.x;

	GroupMemoryBarrierWithGroupSync();

	gHistogram[dispatchThreadId.x] = 0; // reset histogram for next frame

	[unroll]
	for (uint cutoff = (_GROUP_SIZE >> 1); cutoff > 0; cutoff >>= 1) {
		if (dispatchThreadId.x < cutoff) {
			gHistogramShared[dispatchThreadId.x] += gHistogramShared[dispatchThreadId.x + cutoff];
		}

		GroupMemoryBarrierWithGroupSync();
	}

	float sumHistogram = gHistogramShared[0];
	gHistogramShared[dispatchThreadId.x] = 0; // reset shared histogram for next frame

	if (dispatchThreadId.x != 0) {
		return;
	}

	float weightedLogAverage = (sumHistogram / max(kPixelNum - float(countBin), 1.0f)) - 1.0f;
	float weightedAvgLum     = exp2(((weightedLogAverage / 254.0f) * kLogLuminanceRange) + gParameter.minLogLuminance);

	float lumLastFrame = gAverageLuminance[0];
	float adaptedLum   = lumLastFrame + (weightedAvgLum - lumLastFrame) * gParameter.timeCoeff;

	gAverageLuminance[0] = adaptedLum;
}
