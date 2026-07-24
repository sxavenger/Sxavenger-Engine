//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ReSTIR
#include "ReSTIRProcess.hlsli"

//* library
#include "../../../Library/ACES.hlsli"

//* content
#include "../../../Content/Random.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

StructuredBuffer<ReSTIR::Reservoir> gInitialReservoir : register(t0);
RWStructuredBuffer<ReSTIR::Reservoir> gTemporalReservoir  : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	uint p = ReSTIR::FlattenIndex(pixel, dimension);

	ReSTIR::Reservoir s = gInitialReservoir[p];
	ReSTIR::Reservoir r = gTemporalReservoir[p];

	float p_hat = dot(s.sample.lo, ACES::AP1_RGB2Y);
	float p_q   = s.sample.pdf;

	float w = p_q > 0.0f ? p_hat / p_q : 0.0f; //!< 重要度サンプリングの重みを計算 (f(x) / pdf(x))

	Random random;
	random.seed = seed * uint3(pixel + 1, 1);

	r.Update(s.sample, w, random.Generate1d());

	p_hat = dot(r.sample.lo, ACES::AP1_RGB2Y);

	r.w = (p_hat > 0.0 && r.m > 0) ? r.weight / (r.m * p_hat) : 0.0f;
	gTemporalReservoir[p] = r;
	
}
