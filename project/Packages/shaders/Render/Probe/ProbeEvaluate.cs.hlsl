//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ProbeCommon.hlsli"
#include "../DeferredBufferIndex.hlsli"

//* library
#include "../../Library/Math.hlsli"
#include "../../Library/BRDF.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

cbuffer Dimension : register(b0, space1) {
	uint2 dimension;
};

RWTexture2D<float4> gIndirect : register(u0);
ConstantBuffer<DeferredBufferIndexConstantBuffer> gDeferredBufferIndex : register(b0);

ConstantBuffer<CameraComponent> gCamera : register(b1);

//* probe
ConstantBuffer<ProbeConfig> Config : register(b2);
Texture2DArray<float4> gProbeSample     : register(t0);
Texture2DArray<float2> gProbeReservoir  : register(t1);
Texture2DArray<float4> gProbeIrradiance : register(t2);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 GetProbeIndirect(float3 direction, uint3 probeIndex) {
	float2 uv = EncodeOctahedral(normalize(direction));
	float2 section = 1.0f / float2(Config.probeCount.xy);

	float2 probe_uv = (probeIndex.xy + uv) * section;

	float3 color = gProbeSample.SampleLevel(gSampler, float3(probe_uv, probeIndex.z), 0).rgb;
	float w      = gProbeReservoir.SampleLevel(gSampler, float3(probe_uv, probeIndex.z), 0).y;

	float3 irradiance = gProbeIrradiance.SampleLevel(gSampler, float3(probe_uv, probeIndex.z), 0).rgb;

	return irradiance;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	DeferredSurface surface;
	if (!surface.GetSurface(gDeferredBufferIndex.Get(), pixel)) {
		gIndirect[pixel] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; //!< サーフェス情報が無い
	}

	ProbeCage cage;
	if (!ProbeCage::GetCage(Config, surface.position, float3(1, 1, 1), cage)) {
		gIndirect[pixel] = float4(1.0f, 0.0f, 0.0f, 1.0f);
		return; //!< プローブ範囲外
	}

	float weight_sum    = 0.0f;
	float3 indirect_sum = float3(0.0f, 0.0f, 0.0f);

	for (uint i = 0; i < 8; ++i) {
		uint3 probeIndex       = cage.probeIndices[i];
		float3 probePosition   = GetProbePosition(probeIndex, Config, float3(1, 1, 1));
		float3 probeDirection  = normalize(probePosition - surface.position); //!< Surfaceからprobeへの方向ベクトル

		float3 color = GetProbeIndirect(surface.normal, probeIndex);

		float weight_back = max(dot(surface.normal, probeDirection), 0.0f);

		float weight = cage.weight[i];

		indirect_sum += color * weight;
		weight_sum   += weight;
	}

	indirect_sum /= max(weight_sum, kEpsilon);

	gIndirect[pixel] = float4(indirect_sum * surface.albedo / kPi, 1.0f);
}
