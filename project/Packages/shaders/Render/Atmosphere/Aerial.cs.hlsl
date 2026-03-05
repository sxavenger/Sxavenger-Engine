//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyAtmosphere.hlsli"
#include "AtmosphereIntersection.hlsli"

//* library
#include "../../Library/RandomLib.hlsli"
#include "../../Library/Mathmatic.hlsli"
#include "../../Library/Octahedral.hlsli"

//* component
#include "../../Component/TransformComponent.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

RWTexture3D<float4> gAerial : register(u0);

Texture2D<float3> gTransmittance : register(t0);
Texture2D<float3> gMultipleScattering : register(t1);
SamplerState gSampler : register(s0);

ConstantBuffer<TransformComponent> gTransform : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// SingleScattering structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SingleScattering {
	float3 luminance;
	float3 transmittance;
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float GetSampleSegment(float2 uv) {
	return PseudoRandom(uv);
	//return 0.3f;
}

float2 TransmittanceParamToUV(Atmosphere atmosphere, float view_height, float cos_view_zenith) {
	//!< TODO: 共通化
	float height_sq = view_height * view_height;
	float bottom_radius_sq = atmosphere.bottom_radius * atmosphere.bottom_radius;
	float top_radius_sq = atmosphere.top_radius * atmosphere.top_radius;
	float h = sqrt(max(0.0, top_radius_sq - bottom_radius_sq));
	float rho = sqrt(max(0.0, height_sq - bottom_radius_sq));

	float discriminant = height_sq * (cos_view_zenith * cos_view_zenith - 1.0) + top_radius_sq;
	float distance_to_boundary = max(0.0, (-view_height * cos_view_zenith + sqrt(max(discriminant, 0.0))));

	float min_distance = atmosphere.top_radius - view_height;
	float max_distance = rho + h;
	float x_mu = (distance_to_boundary - min_distance) / (max_distance - min_distance);
	float x_r = rho / h;

	return float2(x_mu, x_r);
}

float CornetteShanksPhase(float cos_theta, float g) {
	float k = 3.0 / (8.0 * Mathmatic::kPi) * (1.0 - g * g) / (2.0 + g * g);
	return k * (1.0 + cos_theta * cos_theta) / pow(1.0 + g * g - 2.0 * g * -cos_theta, 1.5);
}

float MiePhase(float cos_theta, float g_or_d) {
	return CornetteShanksPhase(-cos_theta, g_or_d);
}

float RayleighPhase(float cos_theta) {
	float factor = 3.0f / (16.0f * Mathmatic::kPi);
	return factor * (1.0f + cos_theta * cos_theta);
}

float2 FromUnitToSubUV(float2 uv, float2 resolution) {
	return (uv + 0.5 / resolution) * (resolution / (resolution + 1.0));
}

float3 GetMultipleScattering(Atmosphere atmosphere, float3 scattering, float3 extinction, float3 world_pos, float cos_view_zenith) {
	float2 uv = saturate(float2(cos_view_zenith * 0.5 + 0.5, (length(world_pos) - atmosphere.bottom_radius) / (atmosphere.top_radius - atmosphere.bottom_radius)));

	uint2 resolution;
	gMultipleScattering.GetDimensions(resolution.x, resolution.y);
	
	uv = FromUnitToSubUV(uv, resolution);
	return gMultipleScattering.SampleLevel(gSampler, uv, 0);
}

SingleScattering IntergrateScatteredLuminance(float2 uv, float3 world_pos, float3 world_dir, Atmosphere atmosphere, float3 sun_direction, uint sample_count, float tmax_bound) {

	SingleScattering result = (SingleScattering)0;

	float3 planet_center = float3(0.0f, 0.0f, 0.0f);
	
	float t_max = 0.0f;
	if (!FindAtmosphereTMax(t_max, world_pos, world_dir, planet_center, atmosphere.bottom_radius, atmosphere.top_radius)) {
		return result;
	}
	t_max = min(t_max, tmax_bound);

	float sample_segment_t = GetSampleSegment(uv);
	float dt = t_max / sample_count;

	float cos_theta = dot(world_dir, sun_direction);
	float mie_phase_v = MiePhase(cos_theta, atmosphere.mie_phase_param);
	float rayleigh_phase_v = RayleighPhase(cos_theta);

	result.luminance     = 0.0f;
	result.transmittance = 1.0f;
	float t = 0.0;
	float dt_exact = 0.0;

	for (uint s = 0; s < sample_count; ++s) {

		float t_new = (s + sample_segment_t) * dt;
		dt_exact = t_new - t;
		t = t_new;

		float3 sample_pos   = world_pos + t * world_dir;
		float sample_height = length(sample_pos);

		MediumSample medium         = MediumSample::SampleMedium(sample_height - atmosphere.bottom_radius, atmosphere);
		float3 sample_transmittance = exp(-medium.extinction * dt_exact);

		float3 zenith = sample_pos / sample_height;

		float cos_sun_zenith = dot(sun_direction, zenith);
		float3 transmittance_to_sun  = gTransmittance.SampleLevel(gSampler, TransmittanceParamToUV(atmosphere, sample_height, cos_sun_zenith), 0);
		float3 phase_times_scattering = medium.mie_scattering * mie_phase_v + medium.rayleigh_scattering * rayleigh_phase_v;
		float3 multi_scattered_luminance = GetMultipleScattering(atmosphere, medium.scattering, medium.extinction, sample_pos, cos_sun_zenith);
		//float planet_shadow = ComputePlanetShadow(sample_pos, sun_dir, kPlanetCenter + 0.01f * zenith, atmosphere.bottom_radius);
		//float shadow = get_sample_shadow(atmosphere, sample_pos, 0);

		float3 scattered_luminance = atmosphere.intensity * (transmittance_to_sun * phase_times_scattering + multi_scattered_luminance * medium.scattering);

		float3 intergrated_luminance = (scattered_luminance - scattered_luminance * sample_transmittance) / medium.extinction;
		result.luminance     += result.transmittance * intergrated_luminance;
		result.transmittance *= sample_transmittance;
	}

	return result;
}

float ThreadZToSlice(uint thread_z) {
	float slice = (float(thread_z) + 0.5) / dimension.z;
	return (slice * slice) * dimension.z; // squared distribution
}

float AerialPerspectiveSliceToDepth(float slice) {
	static const float kApDistancePerSlice = 100.0f;
	return slice * kApDistancePerSlice;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	if (any(dispatchThreadId >= dimension)) {
		return; //!< out of range
	}

	float2 uv = (float2(dispatchThreadId.xy) + 0.5f) / float2(dimension.xy);

	float3 view_world_pos = float3(0.0f, gAtmosphere.bottom_radius + 0.01f, 0.0f);
	float view_height     = length(view_world_pos);
	float3 world_pos      = float3(0.0, view_height, 0.0);
	float3 world_dir      = DecodeOctahedral(uv);
	//float3 world_dir = normalize(float3(0.0f, 0.0f, 1.0f));

	float t_max            = AerialPerspectiveSliceToDepth(ThreadZToSlice(dispatchThreadId.z));
	float3 slice_start_pos = world_pos + t_max * world_dir;

	if (view_height <= gAtmosphere.bottom_radius + 0.01f) {
		world_dir       = normalize(slice_start_pos - world_pos);
		t_max           = length(slice_start_pos);
	}

	view_height = length(world_pos);
	if (view_height >= gAtmosphere.top_radius)  {
		float3 prev_world_pos = world_pos;
		float distance_to_atmosphere = length(prev_world_pos - world_pos);
		
		if (t_max < distance_to_atmosphere) {
			gAerial[dispatchThreadId] = float4(0.0f, 0.0f, 0.0f, 1.0f);
			return;
		}
		
		t_max = max(0.0, t_max - distance_to_atmosphere);
	}


	uint sample_count = max(1, (dispatchThreadId.z + 1) * 2);
	SingleScattering ss = IntergrateScatteredLuminance(uv, world_pos, world_dir, gAtmosphere, -gTransform.GetForwardDirection(), sample_count, t_max);

	float transmittance = dot(ss.transmittance, 1.0f / 1.0f);
	gAerial[dispatchThreadId] = float4(ss.luminance, 1.0 - transmittance);

}
