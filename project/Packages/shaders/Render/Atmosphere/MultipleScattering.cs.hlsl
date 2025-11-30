//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyAtmosphere.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

#define THREAD_GROUP_SIZE_Z 64

//=========================================================================================
// constant variables
//=========================================================================================

static const uint kSampleCount = 20;

static const float kPlanetOffset = 0.01f;
static const float kIsotropicPhase = 1.0f / (4.0f * kPi);

//=========================================================================================
// shared variables
//=========================================================================================

groupshared float3 shared_multi_scattering[THREAD_GROUP_SIZE_Z];
groupshared float3 shared_luminance[THREAD_GROUP_SIZE_Z];

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float3> gMultipleScattering : register(u0);

Texture2D<float3> gTransmittance        : register(t0);
SamplerState gSampler                   : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// Integration structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Integration {
	float3 multi_scattering;
	float3 luminance;
};


////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float2 TransmittanceParamToUV(Atmosphere atmosphere, float view_height, float cos_view_zenith) {
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

float3 GetTransmittanceToSun(float3 sun_dir, float3 zenith, Atmosphere atmosphere, float sample_height) {
	float cos_sun_zenith = dot(sun_dir, zenith);
	float2 uv = TransmittanceParamToUV(atmosphere, sample_height, cos_sun_zenith);
	return gTransmittance.SampleLevel(gSampler, uv, 0);
}

Integration IntegrateScatteredLuminance(float3 world_pos, float3 world_dir, float3 sun_dir, Atmosphere atmosphere) {

	float3 planet_center = float3(0.0, 0.0, 0.0);
	float t_max    = 0.0f;
	float t_bottom = 0.0f;

	if (!FindAtmosphereTMaxTBottom(t_max, t_bottom, world_pos, world_dir, planet_center, atmosphere.bottom_radius, atmosphere.top_radius)) {
		return (Integration)0;
	}

	Integration result = (Integration)0;

	float sample_segment_t = 0.3;
	float dt               = t_max / kSampleCount;

	float3 throughput = 1.0f;
	float t = 0.0;
	float dt_exact = 0.0;
	
	for (uint i = 0; i < kSampleCount; ++i) {
		float t_new = (i + sample_segment_t) * dt;
		dt_exact = t_new - t;
		t = t_new;

		float3 sample_pos = world_pos + t * world_dir;
		float sample_height = length(sample_pos);

		float3 zenith = sample_pos / sample_height;
		float3 transmittance_to_sun = GetTransmittanceToSun(sun_dir, zenith, atmosphere, sample_height);

		MediumSample medium         = MediumSample::SampleMedium(sample_height - atmosphere.bottom_radius, atmosphere);
		float3 sample_transmittance = exp(-medium.extinction * dt_exact);

		float planet_shadow = ComputePlanetShadow(sample_pos, sun_dir, planet_center + kPlanetOffset * zenith, atmosphere.bottom_radius);
		float3 scattered_luminance = planet_shadow * transmittance_to_sun * (medium.scattering * kIsotropicPhase);

		result.multi_scattering += throughput * (medium.scattering - medium.scattering * sample_transmittance) / medium.extinction;
		result.luminance        += throughput * (scattered_luminance - scattered_luminance * sample_transmittance) / medium.extinction;

		throughput *= sample_transmittance;
	}

	// Account for light bounced off the planet
	if (t_max == t_bottom && t_bottom > 0.0) {
		float t = t_bottom;
		float3 sample_pos = world_pos + t * world_dir;
		float sample_height = length(sample_pos);

		float3 zenith = sample_pos / sample_height;
		float3 transmittance_to_sun = GetTransmittanceToSun(sun_dir, zenith, atmosphere, sample_height);

		float n_dot_l = saturate(dot(zenith, sun_dir));
		result.luminance += transmittance_to_sun * throughput * n_dot_l * atmosphere.ground_albedo / kPi;
	}

	return result;
}

float3 ComputeSampleDirection(uint direction_index) {
	static float golden_ratio = (1.0 + sqrt(5.0)) * 0.5;
	
	float theta     = kTau * direction_index / golden_ratio;
	float phi       = acos(1.0 - 2.0 * (direction_index + 0.5) / THREAD_GROUP_SIZE_Z);
	float cos_phi   = cos(phi);
	float sin_phi   = sin(phi);
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	
	return float3(cos_theta * sin_phi, cos_phi, sin_theta * sin_phi);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(1, 1, THREAD_GROUP_SIZE_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	uint direction = dispatchThreadId.z;

	float2 uv = (float2(pixel) + 0.5) / float2(dimension);

	float cos_sun_zenith = uv.x * 2.0f - 1.0f;
	float3 sun_dir = float3(0.0, cos_sun_zenith, sqrt(saturate(1.0 - cos_sun_zenith * cos_sun_zenith)));
	float view_height = gAtmosphere.bottom_radius + saturate(uv.y + kPlanetOffset) * (gAtmosphere.top_radius - gAtmosphere.bottom_radius - kPlanetOffset);

	float3 world_pos = float3(0.0, view_height, 0.0f);
	float3 world_dir = ComputeSampleDirection(direction);

	Integration scattering_result = IntegrateScatteredLuminance(world_pos, world_dir, normalize(sun_dir), gAtmosphere);

	shared_multi_scattering[direction] = scattering_result.multi_scattering / THREAD_GROUP_SIZE_Z;
	shared_luminance[direction]        = scattering_result.luminance / THREAD_GROUP_SIZE_Z;

	GroupMemoryBarrierWithGroupSync();

	for (uint i = 32u; i > 0; i = i >> 1) {
		if (direction < i) {
			shared_multi_scattering[direction] += shared_multi_scattering[direction + i];
			shared_luminance[direction]        += shared_luminance[direction + i];
		}

		GroupMemoryBarrierWithGroupSync();
	}
	
	if (direction > 0) {
		return;
	}

	float3 luminance = shared_luminance[0] * (1.0 / (1.0 - shared_multi_scattering[0]));

	gMultipleScattering[pixel] = gAtmosphere.multi_scattering_factor * luminance;
	
}
