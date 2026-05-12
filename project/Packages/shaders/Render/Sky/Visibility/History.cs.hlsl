//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyVisibilityCommon.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gHistory : register(u0);

StructuredBuffer<ReservoirLib::Reservoir> gReservoir : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外のスレッドは処理しない
	}

	uint p = ReservoirLib::FlattenIndex(pixel, dimension); //!< 2次元配列のindexを1次元配列のindexに変換

	ReservoirLib::Reservoir reservoir = gReservoir[p];

	float3 previous = gHistory[pixel].rgb;
	float3 current  = reservoir.sample.radiance * reservoir.w;

	float previous_luminance = dot(previous, ACES::AP1_RGB2Y);
	float current_luminance  = dot(current, ACES::AP1_RGB2Y);

	float alpha = 0.0f;

	if (previous_luminance > current_luminance) {
		alpha = 0.85f; //!< 前のフレームの方が明るい場合は、前のフレームを重視する

	} else {
		alpha = 0.95f; //!< 今のフレームの方が明るい場合は、今のフレームを重視する
	}

	gHistory[pixel].rgb = lerp(previous, current, 1.0f - alpha);
	gHistory[pixel].a   = 1.0f;
	
}
