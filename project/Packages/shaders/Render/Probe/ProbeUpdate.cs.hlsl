//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ProbeCommon.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

RWTexture2DArray<float4> gProbeSample     : register(u0);
RWTexture2DArray<float2> gProbeReservoir  : register(u1);
RWTexture2DArray<uint2> gProbeMoment      : register(u2);
RWTexture2DArray<float4> gProbeIrradiance : register(u3);

ConstantBuffer<ProbeConfig> Config : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

Reservoir LoadReservoir(uint2 location, uint slice) {
	uint3 texcoord = uint3(location, slice);

	float4 probe_sample    = gProbeSample.Load(texcoord);
	float2 probe_reservoir = gProbeReservoir.Load(texcoord);
	uint probe_moment      = gProbeMoment.Load(texcoord).x;

	Reservoir reservoir;
	reservoir.sample.lo  = probe_sample.rgb;
	reservoir.sample.pdf = probe_sample.a;
	reservoir.weight_sum = probe_reservoir.x;
	reservoir.w          = probe_reservoir.y;
	reservoir.m          = probe_moment.x;

	return reservoir;
}

void SaveReservoir(uint2 location, uint slice, Reservoir reservoir) {
	uint3 texcoord = uint3(location, slice);

	float4 probe_sample    = float4(reservoir.sample.lo, reservoir.sample.pdf);
	float2 probe_reservoir = float2(reservoir.weight_sum, reservoir.w);
	uint probe_moment      = reservoir.m;

	gProbeSample[texcoord]    = probe_sample;
	gProbeReservoir[texcoord] = probe_reservoir;
	gProbeMoment[texcoord].x  = probe_moment;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 probeBufferIndex = dispatchThreadId.xy;

	uint2 probeBufferDimension = uint2(Config.probeResolution * Config.probeCount.xy);
	uint3 probeDimension       = uint3(probeBufferDimension, Config.probeCount.z);

	if (any(probeBufferIndex >= probeBufferDimension) || dispatchThreadId.z >= Config.probeCount.z) {
		return; //!< 範囲外
	}

	Reservoir reservoir = LoadReservoir(probeBufferIndex, dispatchThreadId.z);

	if (reservoir.m < Config.sampleCount) {
		return; //!< sample数が上限に達していない場合は終了.
	}

	//!< irradianceの更新
	float3 irradiance = gProbeIrradiance.Load(uint3(probeBufferIndex, dispatchThreadId.z)).xyz;
	irradiance = lerp(irradiance, reservoir.sample.lo * reservoir.w, 1.0f - Config.hysteresis);

	gProbeIrradiance[uint3(probeBufferIndex, dispatchThreadId.z)] = float4(irradiance, 1.0f);

	// reserviorの初期化
	reservoir = (Reservoir)0;
	SaveReservoir(probeBufferIndex, dispatchThreadId.z, reservoir);

}
