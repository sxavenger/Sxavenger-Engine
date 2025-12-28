//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ProbePathTracing.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
RAYGENERATION void mainRaygeneration() {

	uint3 dispatchIndex     = DispatchRaysIndex();
	uint3 dispatchDimension = DispatchRaysDimensions();
	//!< dimension想定: probe_resolution.xy * probe_count.xy, probe_count.z

	Random random;
	random.seed = seed * (dispatchIndex.xyz + 1);

	Reservoir reservoir = LoadReservoir(dispatchIndex.xy, dispatchIndex.z);
	
	float2 probeLocation = (dispatchIndex.xy % Config.probeResolution) / float2(Config.probeResolution);
	float3 direction      = DecodeOctahedral(probeLocation);

	uint3 probeIndex = dispatchIndex / uint3(Config.probeResolution, 1);

	if (reservoir.m >= Config.sampleCount) {
		return; //!< sample数が上限に達している場合は終了.
	}

	Reservoir newReservoir = (Reservoir)0;

	{ //!< generate new reservior sample

		float2 xi = Hammersley(reservoir.m, Config.sampleCount);
		float3 wi = ImportanceSampleCosineWeight(xi, direction);

		Payload payload = TracePrimaryRay(GetProbePosition(probeIndex, Config, gCamera.GetPosition()), wi);

		Sample sample;
		sample.lo  = payload.lo;
		sample.pdf = ImportanceSampleCosineWeightPDF(wi, direction);

		float w = dot(sample.lo, ACES::AP1_RGB2Y) / max(sample.pdf, kEpsilon);

		newReservoir.Update(sample, w, random.Generate1d());
	}

	{ //!< temporal reservoir update
		float p_hat = dot(newReservoir.sample.lo, ACES::AP1_RGB2Y);
		float p_q   = newReservoir.sample.pdf;
		float w     = p_q > 0.0f ? p_hat * rcp(p_q) : 0.0f;

		reservoir.Update(newReservoir.sample, w, random.Generate1d());

		p_hat = dot(reservoir.sample.lo, ACES::AP1_RGB2Y);
		reservoir.w = (p_hat > 0.0 && reservoir.m > 0) ? reservoir.weight_sum / (reservoir.m * p_hat) : 0.0f;
	}

	SaveReservoir(dispatchIndex.xy, dispatchIndex.z, reservoir);
}

