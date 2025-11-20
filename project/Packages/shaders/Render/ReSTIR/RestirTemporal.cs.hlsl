//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RestirPass.hlsli"

//* library
#include "../../Library/ACES.hlsli"

//* content
#include "../../Content/Random.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

RWStructuredBuffer<Reservoir> gInitalizeReservoir : register(u0);
RWStructuredBuffer<Reservoir> gTemporalReservoir  : register(u1);
//!< todo: TemporalReservoirをPath Tracing Passで使う.

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREAD_X, _NUM_THREAD_Y, _NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	uint p = GetIndex1d(pixel);

	Reservoir s = gInitalizeReservoir[p];
	Reservoir r = gTemporalReservoir[p];

	float p_hat = dot(s.sample.lo, ACES::AP1_RGB2Y);
	float p_q   = s.sample.pdf;

	float w = p_q > 0.0f ? p_hat * rcp(p_q) : 0.0f;

	Random random;
	random.seed = seed * uint3(pixel + 1, 1);

	r.Update(s.sample, w, random.Generate1d());

	p_hat = dot(r.sample.lo, ACES::AP1_RGB2Y);

	r.w = (p_hat > 0.0 && r.m > 0) ? r.weight * rcp(r.m * p_hat) : 0.0f;
	gTemporalReservoir[p] = r;

}
