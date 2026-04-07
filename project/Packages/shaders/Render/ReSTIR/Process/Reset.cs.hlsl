//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ReSTIRProcess.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

RWStructuredBuffer<ReSTIR::Reservoir> gTemporalReservoir : register(u0);
RWStructuredBuffer<ReSTIR::Reservoir> gSpatialReservoir  : register(u1);
RWStructuredBuffer<ReSTIR::Moment>    gMoment            : register(u2);

Texture2D<float4> gMotionVector : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	float2 motion = gMotionVector.Load(uint3(pixel, 0)).xy;

	if (all(motion == 0.0f)) {
		return; //!< 静止画素はリセットしない
	}

	uint p = ReSTIR::FlattenIndex(pixel, dimension);

	gTemporalReservoir[p] = (ReSTIR::Reservoir)0;
	gSpatialReservoir[p]  = (ReSTIR::Reservoir)0;
	gMoment[p]            = ReSTIR::Moment::GetDefault(pixel);
	
}
