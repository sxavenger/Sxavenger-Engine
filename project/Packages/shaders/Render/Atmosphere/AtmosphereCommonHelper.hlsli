#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// AtmosphereParameters structure
////////////////////////////////////////////////////////////////////////////////////////////
struct AtmosphereParameters {
	float top_radius;
	float bottom_radius;
	float3 rayleigh_scattering;
	float3 mie_scattering;
	float rayleigh_scale_height;
	float mie_scale_height;
	float mie_extinction;
	float3 absorption_extinction;
	float absorption_extinction_scale_height;
	float solar_irradiance;
	float mu_s_min;
	float mie_phase_function_g;
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float ClampCosine(float mu) {
	return clamp(mu, -1.0, 1.0);
}

float ClampDistance(float d) {
	return max(d, 0.0);
}

float ClampRadius(AtmosphereParameters atmosphere, float r) {
	return clamp(r, atmosphere.bottom_radius, atmosphere.top_radius);
}

float SafeSqrt(float a) {
	return sqrt(max(a, 0.0));
}

float DistanceToTopAtmosphereBoundary(AtmosphereParameters atmosphere, float r, float mu) {
	//assert(r <= atmosphere.top_radius);
	//assert(mu >= -1.0 && mu <= 1.0);
	float discriminant = r * r * (mu * mu - 1.0) + atmosphere.top_radius * atmosphere.top_radius;
	return ClampDistance(-r * mu + SafeSqrt(discriminant));
}

float DistanceToBottomAtmosphereBoundary(AtmosphereParameters atmosphere, float r, float mu) {
	//assert(r >= atmosphere.bottom_radius);
	//assert(mu >= -1.0 && mu <= 1.0);
	float discriminant = r * r * (mu * mu - 1.0) + atmosphere.bottom_radius * atmosphere.bottom_radius;
	return ClampDistance(-r * mu - SafeSqrt(discriminant));
}

bool RayIntersectsGround(AtmosphereParameters atmosphere, float r, float mu, out float d_1, out float d_2) {
	//assert(r >= atmosphere.bottom_radius);
	//assert(mu >= -1.0 && mu <= 1.0);
	d_1 = 0.0;
	d_2 = 0.0;
	
	float discriminant = 4 * r * r * (mu * mu - 1.0) + 4 * atmosphere.bottom_radius * atmosphere.bottom_radius;
	
	if (discriminant >= 0.0f) {
		float sqDis = sqrt(discriminant);
		d_1 = (-2.0f * r * mu - sqDis) / 2.0f;
		d_2 = (-2.0f * r * mu + sqDis) / 2.0f;
	}
	
	return mu < 0.0 && discriminant >= 0.0;
}

bool RayIntersectsGround(AtmosphereParameters atmosphere, float r, float mu) {
	//assert(r >= atmosphere.bottom_radius);
	//assert(mu >= -1.0 && mu <= 1.0);
	return mu < 0.0 && r * r * (mu * mu - 1.0) + atmosphere.bottom_radius * atmosphere.bottom_radius >= 0.0;
}

float GetTextureCoordFromUnitRange(float x, uint texture_size) {
	return 0.5 / texture_size + x * (1.0 - 1.0 / texture_size);
}

float GetUnitRangeFromTextureCoord(float u, uint texture_size) {
	return (u - 0.5 / texture_size) / (1.0 - 1.0 / texture_size);
}

float GetScaleHeight(float altitude, float scale_height) {
	return exp(-altitude / scale_height);
}

float2 GetScaleHeight(float altitude, float2 scale_height_rayleigh_mie) {
	return exp(-altitude / scale_height_rayleigh_mie);
}

float DistanceToNearestAtmosphereBoundary(AtmosphereParameters atmosphere, float r, float mu, bool ray_r_mu_intersects_ground) {
	if (ray_r_mu_intersects_ground) {
		return DistanceToBottomAtmosphereBoundary(atmosphere, r, mu);
		
	} else {
		return DistanceToTopAtmosphereBoundary(atmosphere, r, mu);
	}
}

float GetNuFromMuMus(float mu, float mu_s) {
	return /*cos(a-b), where cos(a) == mu, cos(b) == mu_s*/ mu * mu_s + (1 - mu * mu) * (1 - mu_s * mu_s);
}
