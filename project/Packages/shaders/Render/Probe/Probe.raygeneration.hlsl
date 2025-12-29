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
		return; //!<
	}

	Reservoir newReservoir = (Reservoir)0;

	{ 
		float2 xi = Hammersley(reservoir.m, Config.sampleCount);
		float3 wi = ImportanceSampleCosineWeight(xi, direction);
		
		Payload payload = TracePrimaryRay(GetProbePosition(probeIndex, Config, float3(1, 1, 1)), wi);

		Sample sample = (Sample)0;
		sample.lo  = payload.lo * max(dot(wi, direction), 0.0f);
		sample.pdf = ImportanceSampleCosineWeightPDF(wi, direction);

		newReservoir.Update(sample, dot(sample.lo, ACES::AP1_RGB2Y) / max(sample.pdf, kEpsilon), random.Generate1d());
	}

	{
		float p_hat = dot(newReservoir.sample.lo, ACES::AP1_RGB2Y);
		float p_q   = newReservoir.sample.pdf;

		float w = p_q > 0.0f ? p_hat * rcp(p_q) : 0.0f;
		reservoir.Update(newReservoir.sample, w, random.Generate1d());

		p_hat = dot(reservoir.sample.lo, ACES::AP1_RGB2Y);
		reservoir.w = (p_hat > 0.0 && reservoir.m > 0) ? reservoir.weight_sum / (reservoir.m * p_hat) : 0.0f;
	}

	SaveReservoir(dispatchIndex.xy, dispatchIndex.z, reservoir);
}

