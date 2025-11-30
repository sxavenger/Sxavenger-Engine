//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RestirPass.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

RWStructuredBuffer<Reservoir> gTemporalReservoir : register(u0);
RWStructuredBuffer<Reservoir> gSpatialReservoir  : register(u1);
RWStructuredBuffer<Moment>    gMoment            : register(u2);

Texture2D<float4> gVelocity : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREAD_X, _NUM_THREAD_Y, _NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}


	float2 velocity = gVelocity.Load(uint3(pixel, 0)).xy;

	if (all(velocity == 0.0f)) {
		return; //!< 静止画素はリセットしない
	}

	uint p = Flatten(pixel);

	gTemporalReservoir[p] = (Reservoir)0;
	gSpatialReservoir[p]  = (Reservoir)0;
	gMoment[p]            = Moment::GetDefault(pixel);

}
