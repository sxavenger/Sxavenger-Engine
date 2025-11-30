//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyAtmosphere.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float3> gTransmittance : register(u0);

//=========================================================================================
// constant variables
//=========================================================================================

static const uint kSampleCount = 40;

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float2 TransmittanceParam(float2 uv, Atmosphere atmosphere) {
	float x_mu = uv.x;
	float x_r  = uv.y;

	float bottom_radius_sq = atmosphere.bottom_radius * atmosphere.bottom_radius;
	
	float h_sq        = atmosphere.top_radius * atmosphere.top_radius - bottom_radius_sq;
	float h           = sqrt(h_sq);
	float rho         = h * x_r;
	float rho_sq      = rho * rho;
	float view_height = sqrt(rho_sq + bottom_radius_sq);

	float d_min = atmosphere.top_radius - view_height;
	float d_max = rho + h;
	float d     = d_min + x_mu * (d_max - d_min);

	float cos_view_zenith = 1.0;
	
	if (d != 0.0) {
		cos_view_zenith = clamp((h_sq - rho_sq - d * d) / (2.0 * view_height * d), -1.0, 1.0);
	}

	return float2(view_height, cos_view_zenith);
	
}

float3 SampleMediumExtinction(float h, Atmosphere atmosphere) {
	float mie_density      = exp(atmosphere.mie_density_exp_scale * h);
	float rayleigh_density = exp(atmosphere.rayleigh_density_exp_scale * h);

	float absorption_density;
	if (h < atmosphere.absorption_density_0_layer_height) {
		absorption_density = saturate(atmosphere.absorption_density_0_linear_term * h + atmosphere.absorption_density_0_constant_term);
	} else {
		absorption_density = saturate(atmosphere.absorption_density_1_linear_term * h + atmosphere.absorption_density_1_constant_term);
	}

	float3 mie_extinction        = mie_density * atmosphere.mie_extinction;
	float3 rayleigh_extinction   = rayleigh_density * atmosphere.rayleigh_scattering;
	float3 absorption_extinction = absorption_density * atmosphere.absorption_extinction;

	return mie_extinction + rayleigh_extinction + absorption_extinction;
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

	float2 uv = (float2(pixel) + 0.5f) / float2(dimension);

	float2 lut_params     = TransmittanceParam(uv, gAtmosphere);
	float view_height     = lut_params.x;
	float cos_view_zenith = lut_params.y;

	float2 world_pos = float2(0.0, view_height);
	float2 world_dir = float2(sqrt(max(1.0 - cos_view_zenith * cos_view_zenith, 0.0)), cos_view_zenith);

	float3 transmittance = 0.0f;

	float t_max = 0.0f;
	if (FindIntersectionTMax(t_max, world_pos, world_dir, gAtmosphere.bottom_radius, gAtmosphere.top_radius)) {

		float sample_segment_t = 0.3f;
		float dt               = t_max / kSampleCount;

		float t        = 0.0f;
		float dt_exact = 0.0f;

		for (uint i = 0; i < kSampleCount; ++i) {
			float t_new = (i + sample_segment_t) * dt;

			dt_exact = t_new - t;
			t        = t_new;
			
			float sample_height = length(world_pos + t * world_dir) - gAtmosphere.bottom_radius;
			transmittance += SampleMediumExtinction(sample_height, gAtmosphere) * dt_exact;
		}

		transmittance = exp(-transmittance);
	}

	gTransmittance[pixel] = transmittance;
}
