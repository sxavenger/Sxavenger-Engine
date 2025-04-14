#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../Library/Math.hlsli"
#include "../Library/Photometry.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// PointLightComponent structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PointLightComponent {
	
	//=========================================================================================
	// public variables
	//=========================================================================================

	float3 color;
	LightUnits::Type unit;
	float intensity;
	float radius;
	
	//=========================================================================================
	// public methods
	//=========================================================================================

	float GetRadiance(float r) {
		float radiance = 0.0f;
	
		switch (unit) {
			case LightUnits::Lumen:
				radiance = intensity / (4.0f * kPi);
				break;

			case LightUnits::Candela:
				radiance = intensity;
				break;
		};

		float dist = pow(saturate(1.0f - pow(r / radius, 4.0f)), 2.0f) / (r * r + 1.0f); //!< dist = func_win(r);

		return radiance * dist;
	}

	float3 GetColor(float r) {
		return color * GetRadiance(r);
	}
	
};
