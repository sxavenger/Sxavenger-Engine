//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyAtmosphere.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float GetUnitRangeFromTextureCoord(float u, uint size) {
	return (u - 0.5 / size) / (1.0 - 1.0 / size);
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

	float x_mu = GetUnitRangeFromTextureCoord(uv.x, dimension.x);
	float x_r  = GetUnitRangeFromTextureCoord(uv.y, dimension.y);

	// Distance to top atmosphere boundary for a horizontal ray at ground level.
	float h = sqrt(gAtmosphere.atmosphere_radius * gAtmosphere.atmosphere_radius - gAtmosphere.planet_radius * gAtmosphere.planet_radius);

	// Distance to the horizon, from which we can compute r:
	float rho = h * x_r;
	float r = sqrt(rho * rho + gAtmosphere.planet_radius * gAtmosphere.planet_radius);
	
	// Distance to the top atmosphere boundary for the ray (r,mu), and its minimum
	// and maximum values over all mu - obtained for (r,1) and (r,mu_horizon) -
	// from which we can recover mu:
	float d_min = gAtmosphere.atmosphere_radius - r;
	float d_max = rho + H;
	float d = d_min + x_mu * (d_max - d_min);
	
	float mu = d == 0.0 ? 1.0 : (h * h - rho * rho - d * d) / (2.0 * r * d);
	mu = clamp(mu, -1.0, 1.0);
	
	
}
