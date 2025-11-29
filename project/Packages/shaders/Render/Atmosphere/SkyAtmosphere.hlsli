#pragma once

/* reference
 - [Unreal Engine Physically based Atmosphere](https://sebh.github.io/publications/egsr2020.pdf)
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "AtmosphereIntersection.hlsli"

//* library
#include "../../Library/Math.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

//* shaders
#define _NUM_THREAD_X 16
#define _NUM_THREAD_Y 16
#define _NUM_THREAD_Z 1

////////////////////////////////////////////////////////////////////////////////////////////
// Atmosphere structures
////////////////////////////////////////////////////////////////////////////////////////////
struct Atmosphere {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float3 rayleigh_scattering;
	float rayleigh_density_exp_scale;

	float3 absorption_extinction;
	float absorption_density_0_layer_height;
	float absorption_density_0_linear_term;
	float absorption_density_0_constant_term;
	float absorption_density_1_linear_term;
	float absorption_density_1_constant_term;

	float3 mie_scattering;
	float mie_density_exp_scale;
	float3 mie_extinction;

	float top_radius;
	float bottom_radius;

	float3 ground_albedo;
	float multi_scattering_factor;
};

//=========================================================================================
// buffer
//=========================================================================================

cbuffer Dimension : register(b0, space1) {
	uint2 dimension;
}

ConstantBuffer<Atmosphere> gAtmosphere : register(b1, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// MediumSample structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MediumSample {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float3 scattering;
	float3 extinction;

	float3 mie_scattering;
	float3 rayleigh_scattering;

	//=========================================================================================
	// public methods
	//=========================================================================================

	static MediumSample SampleMedium(float h, Atmosphere atmosphere) {
		float mie_density      = exp(atmosphere.mie_density_exp_scale * h);
		float rayleigh_density = exp(atmosphere.rayleigh_density_exp_scale * h);
		float absorption_density;
	
		if (h < atmosphere.absorption_density_0_layer_height) {
			absorption_density = saturate(atmosphere.absorption_density_0_linear_term * h + atmosphere.absorption_density_0_constant_term);

		} else {
			absorption_density = saturate(atmosphere.absorption_density_1_linear_term * h + atmosphere.absorption_density_1_constant_term);
		}

		MediumSample s;
		s.mie_scattering      = mie_density * atmosphere.mie_scattering;
		s.rayleigh_scattering = rayleigh_density * atmosphere.rayleigh_scattering;
		s.scattering          = s.mie_scattering + s.rayleigh_scattering;

		float3 mie_extinction        = mie_density * atmosphere.mie_extinction;
		float3 rayleigh_extinction   = s.rayleigh_scattering;
		float3 absorption_extinction = absorption_density * atmosphere.absorption_extinction;
		s.extinction = mie_extinction + rayleigh_extinction + absorption_extinction;

		return s;
	}
	
};
