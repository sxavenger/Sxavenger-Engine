//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyAtmosphere.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float3> gTransmittance : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

void GetRMuFromTransmittanceTextureUv(AtmosphereParameters atmosphere, float2 uv, out float r, out float mu, uint2 texture_size) {
	// assert(uv.x >= 0.0 && uv.x <= 1.0);
	// assert(uv.y >= 0.0 && uv.y <= 1.0);
	float x_mu = GetUnitRangeFromTextureCoord(uv.x, texture_size.x);
	float x_r  = GetUnitRangeFromTextureCoord(uv.y, texture_size.y);
	
	// Distance to top atmosphere boundary for a horizontal ray at ground level.
	float H = sqrt(atmosphere.top_radius * atmosphere.top_radius - atmosphere.bottom_radius * atmosphere.bottom_radius);

	// Distance to the horizon, from which we can compute r:
	float rho = H * x_r;
	r = sqrt(rho * rho + atmosphere.bottom_radius * atmosphere.bottom_radius);
	
	// Distance to the top atmosphere boundary for the ray (r,mu), and its minimum
	// and maximum values over all mu - obtained for (r,1) and (r,mu_horizon) -
	// from which we can recover mu:
	float d_min = atmosphere.top_radius - r;
	float d_max = rho + H;
	float d = d_min + x_mu * (d_max - d_min);
	mu = d == 0.0 ? 1.0 : (H * H - rho * rho - d * d) / (2.0 * r * d);
	mu = ClampCosine(mu);
}

float ComputeOpticalLengthToTopAtmosphereBoundary(AtmosphereParameters atmosphere, float r, float mu, float scale_height) {
	//assert(r >= atmosphere.bottom_radius && r <= atmosphere.top_radius);
	//assert(mu >= -1.0 && mu <= 1.0);
	
	// Number of intervals for the numerical integration.
	const int SAMPLE_COUNT = 40;
	
	// The integration step, i.e. the length of each integration interval.
	float dx = DistanceToTopAtmosphereBoundary(atmosphere, r, mu) / SAMPLE_COUNT;
	
	// Integration loop.
	float result = 0.0;
	for (int i = 0; i <= SAMPLE_COUNT; ++i) {
		float d_i = i * dx;
		
		// Distance between the current sample point and the planet center.
		float r_i = sqrt(d_i * d_i + 2.0 * r * mu * d_i + r * r);
		
		// Number density at the current sample point (divided by the number density
		// at the bottom of the atmosphere, yielding a dimensionless number).
		float y_i = GetScaleHeight(r_i - atmosphere.bottom_radius, scale_height);
		
		// Sample weight (from the trapezoidal rule).
		float weight_i = i == 0 || i == SAMPLE_COUNT ? 0.5 : 1.0;
		result += y_i * weight_i * dx;
	}
	
	return result;
}

float3 ComputeTransmittanceToTopAtmosphereBoundary(AtmosphereParameters atmosphere, float r, float mu) {
	//assert(r >= atmosphere.bottom_radius && r <= atmosphere.top_radius);
	//assert(mu >= -1.0 && mu <= 1.0);
	
	return exp(-(
		atmosphere.rayleigh_scattering *
		ComputeOpticalLengthToTopAtmosphereBoundary(
			atmosphere, r, mu, atmosphere.rayleigh_scale_height) +
		atmosphere.mie_extinction *
		ComputeOpticalLengthToTopAtmosphereBoundary(
			atmosphere, r, mu, atmosphere.mie_scale_height) +
		atmosphere.absorption_extinction *
		ComputeOpticalLengthToTopAtmosphereBoundary(
			atmosphere, r, mu, atmosphere.absorption_extinction_scale_height)
		)
	);
}

float3 ComputeTransmittanceToTopAtmosphereBoundaryTexture(AtmosphereParameters atmosphere, float2 uv_coord, uint2 tex_size) {
	float r;
	float mu;
	GetRMuFromTransmittanceTextureUv(atmosphere, uv_coord, r, mu, tex_size);
	return ComputeTransmittanceToTopAtmosphereBoundary(atmosphere, r, mu);
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

	float2 uv = float2((pixel + 0.5) / dimension);
	float3 result = ComputeTransmittanceToTopAtmosphereBoundaryTexture(Get(), uv, dimension);

	gTransmittance[pixel] = result;
	
}
