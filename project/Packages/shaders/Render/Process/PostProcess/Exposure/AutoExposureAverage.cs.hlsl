//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "AutoExposure.hlsli"

//=========================================================================================
// constant
//=========================================================================================

static const uint kPixelNum = size.x * size.y;

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_GROUP_SIZE, 1, 1)]
void main(uint3 groupThreadId : SV_GroupThreadID) {

	gHistogram[groupThreadId.x] = gHistogramShared[groupThreadId.x];

	GroupMemoryBarrierWithGroupSync();

	uint countBin = gHistogram[groupThreadId.x];
	gHistogramShared[groupThreadId.x] = countBin * groupThreadId.x;

	GroupMemoryBarrierWithGroupSync();

	[unroll]
	for (uint cutoff = (_GROUP_SIZE >> 1); cutoff > 0; cutoff >>= 1) {
		if (groupThreadId.x < cutoff) {
			gHistogramShared[groupThreadId.x] += gHistogramShared[groupThreadId.x + cutoff];
		}

		GroupMemoryBarrierWithGroupSync();
	}

	float sumHistogram = gHistogramShared[0];
	gHistogramShared[groupThreadId.x] = 0; // reset shared histogram for next frame

	if (groupThreadId.x != 0) {
		return;
	}

	float weightedLogAverage = (sumHistogram / max(kPixelNum - float(countBin), 1.0f)) - 1.0f;
	float weightedAvgLum     = exp2(((weightedLogAverage / 254.0f) * kLogLuminanceRange) + gParameter.minLogLuminance);

	float lumLastFrame = gAverageLuminance[0];
	float adaptedLum   = lumLastFrame + (weightedAvgLum - lumLastFrame) * gParameter.timeCoeff;

	gAverageLuminance[0] = adaptedLum;
}
