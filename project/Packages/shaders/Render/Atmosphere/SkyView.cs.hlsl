//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyAtmosphere.hlsli"

//* component
#include "../../Component/TransformComponent.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

RWTexture2D<float4> gSkyView : register(u0);

Texture2D<float3> gTransmittance      : register(t0);
Texture2D<float4> gMultipleScattering : register(t1);
SamplerState gSampler                 : register(s0);

ConstantBuffer<TransformComponent> gTransform : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// SigngleScattering structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SingleScattering {
	float3 luminance;
	float3 transmittance;
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////


float2 FromUnitToSubUV(float2 uv, float2 resolution) {
	return (uv + 0.5 / resolution) * (resolution / (resolution + 1.0));
}

float2 FromSubUVToUnit(float2 uv, float2 resolution) {
	return (uv - 0.5 / resolution) * (resolution / (resolution - 1.0));
}

float3 ComputeSunDir(float3 sun_dir, float3 zenith) {
	float cos_sun_zenith = dot(zenith, sun_dir);
	float sin_theta = sqrt(max(1.0 - cos_sun_zenith * cos_sun_zenith, 0.0));
	return normalize(float3(0.0, cos_sun_zenith, sin_theta));
}

float3 ComputeWorldDir(float2 uv, float view_height, Atmosphere atmosphere) {
	uv = FromSubUVToUnit(uv, float2(dimension));

	float v_horizon = sqrt(max(view_height * view_height - atmosphere.bottom_radius * atmosphere.bottom_radius, 0.0));
	float ground_to_horizon_angle = acos(v_horizon / view_height);
	float zenith_horizon_angle = kPi - ground_to_horizon_angle;

	float cos_view_zenith;
	if (uv.y < 0.5) {
		float coord = 1.0 - (2.0 * uv.y);
		cos_view_zenith = cos(zenith_horizon_angle * (1.0 - (coord * coord)));
		
	} else {
		float coord = (uv.y * 2.0) - 1.0;
		cos_view_zenith = cos(zenith_horizon_angle + ground_to_horizon_angle * (coord * coord));
	}
	
	float sin_view_zenith = sqrt(max(1.0 - cos_view_zenith * cos_view_zenith, 0.0));

	float cos_light_view = -((uv.x * uv.x) * 2.0 - 1.0);
	return float3(
			sin_view_zenith * cos_light_view,
			cos_view_zenith,
			sin_view_zenith * sqrt(max(1.0 - cos_light_view * cos_light_view, 0.0))
		);
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
	float k = 3.0 / (8.0 * kPi) * (1.0 - g * g) / (2.0 + g * g);
	return k * (1.0 + cos_theta * cos_theta) / pow(1.0 + g * g - 2.0 * g * -cos_theta, 1.5);
}

float MiePhase(float cos_theta, float g_or_d) {
	return CornetteShanksPhase(-cos_theta, g_or_d);
}

float RayleighPhase(float cos_theta) {
	float factor = 3.0f / (16.0f * kPi);
	return factor * (1.0f + cos_theta * cos_theta);
}

float3 GetMultipleScattering(Atmosphere atmosphere, float3 scattering, float3 extinction, float3 world_pos, float cos_view_zenith) {
	float2 uv = saturate(float2(cos_view_zenith * 0.5 + 0.5, (length(world_pos) - atmosphere.bottom_radius) / (atmosphere.top_radius - atmosphere.bottom_radius)));

	uint2 resolution;
	gMultipleScattering.GetDimensions(resolution.x, resolution.y);
	
	uv = FromUnitToSubUV(uv, resolution);
	return gMultipleScattering.SampleLevel(gSampler, uv, 0).xyz;
}

SingleScattering IntegrateScatteregLuminance(float3 world_pos, float3 world_dir, float3 sun_dir, Atmosphere atmosphere) {

	const float3 kPlanetCenter = float3(0.0f, 0.0f, 0.0f);

	float t_max = 0.0f;
	
	if (!FindAtmosphereTMax(t_max, world_pos, world_dir, kPlanetCenter, atmosphere.bottom_radius, atmosphere.top_radius)) {
		return (SingleScattering)0;
	}

	const float kMinSpp = 14.0f;
	const float kMaxSpp = 30.0f;

	float sample_count             = lerp(kMinSpp, kMaxSpp, saturate(t_max / 100.0f));
	float sample_count_floored     = floor(sample_count);
	float inv_sample_count_floored = 1.0f / sample_count_floored;
	float t_max_floored            = t_max * sample_count_floored / sample_count;
	float sample_segment_t         = 0.3f;

	const float mie_phase_param = 0.8f;

	float cos_theta          = dot(sun_dir, world_dir);
	float mie_phase_val      = MiePhase(cos_theta, mie_phase_param);
	float rayleigh_phase_val = RayleighPhase(cos_theta);

	SingleScattering result;
	result.luminance      = float3(0.0f, 0.0f, 0.0f);
	result.transmittance  = float3(1.0f, 1.0f, 1.0f);

	float t = 0.0f;
	float dt = t_max / sample_count;


	for (uint i = 0; i < uint(sample_count); ++i) {
		float t0 = i * inv_sample_count_floored;
		float t1 = (i + 1.0) * inv_sample_count_floored;
		t0 = (t0 * t0) * t_max_floored;
		t1 = t1 * t1;

		if (t1 > 1.0) {
			t1 = t_max;
			
		} else {
			t1 = t_max_floored * t1;
		}
		
		dt = t1 - t0;
		t  = t0 + dt * sample_segment_t;

		float3 sample_pos   = world_pos + t * world_dir;
		float sample_height = length(sample_pos);

		MediumSample medium         = MediumSample::SampleMedium(sample_height - atmosphere.bottom_radius, atmosphere);
		float3 sample_transmittance = exp(-medium.extinction * dt);

		float3 zenith = sample_pos / sample_height;

		float cos_sun_zenith             = dot(sun_dir, zenith);
		float3 transmittance_to_sun      = gTransmittance.SampleLevel(gSampler, TransmittanceParamToUV(atmosphere, sample_height, cos_sun_zenith), 0);
		float3 phase_times_scattering    = medium.mie_scattering * mie_phase_val + medium.rayleigh_scattering * rayleigh_phase_val;
		float3 multi_scattered_luminance = GetMultipleScattering(atmosphere, medium.scattering, medium.extinction, sample_pos, cos_sun_zenith);
		//float planet_shadow = ComputePlanetShadow(sample_pos, sun_dir, kPlanetCenter + 0.01f * zenith, atmosphere.bottom_radius);
		//float shadow = get_sample_shadow(atmosphere, sample_pos, 0);

		float3 scattered_luminance = 1.0f * (transmittance_to_sun * phase_times_scattering + multi_scattered_luminance * medium.scattering);

		float3 intergrated_luminance = (scattered_luminance - scattered_luminance * sample_transmittance) / medium.extinction;
		result.luminance     += result.transmittance * intergrated_luminance;
		result.transmittance *= sample_transmittance;
	}

	return result;
	
}
	

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREAD_X, _NUM_THREAD_Y, _NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	float2 uv = (float2(pixel) + 0.5) / float2(dimension);

	float3 view_world_pos = float3(0.0f, gAtmosphere.bottom_radius + 0.01f, 0.0f);
	float view_height     = length(view_world_pos);
	float3 world_pos      = float3(0.0, view_height, 0.0);
	float3 world_dir      = ComputeWorldDir(uv, view_height, gAtmosphere);

	float3 zenith  = view_world_pos / view_height;
	float3 sun_dir = ComputeSunDir(gTransform.GetDirection(), zenith);

	SingleScattering ss = IntegrateScatteregLuminance(world_pos, world_dir, sun_dir, gAtmosphere);

	gSkyView[pixel] = float4(ss.luminance, 1.0f);
	
}
