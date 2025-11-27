#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "AtmosphereCommonHelper.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

//* shaders
#define _NUM_THREAD_X 16
#define _NUM_THREAD_Y 16
#define _NUM_THREAD_Z 1

AtmosphereParameters Get() {
		AtmosphereParameters result;
		
		result.top_radius                         = 6460000.0f;
		result.bottom_radius                      = 6360000.0f;
		result.rayleigh_scattering                = float3(5.802e-6f, 13.558e-6f, 33.1e-6f);
		result.rayleigh_scale_height              = 8000.0f;
		result.mie_scattering                     = 3.996e-6f;
		result.mie_extinction                     = 4.40e-6f;
		result.mie_scale_height                   = 1200.0f;
		result.mie_phase_function_g               = 0.8f;
		result.absorption_extinction              = float3(0.650e-6f, 1.881e-6f, 0.085e-6f);
		result.absorption_extinction_scale_height = 30000.0f;

		result.solar_irradiance = 1.0f;
		result.mu_s_min         = -0.2f;
		
		return result;
	}

//=========================================================================================
// buffer
//=========================================================================================

cbuffer Dimension : register(b0, space1) {
	uint2 dimension;
}

