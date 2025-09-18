#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "LightComponentCommon.hlsli"

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

	InlineShadow shadow;

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
				radiance = intensity / (4.0f * kPi) * kPi;
				break;

			case LightUnits::Candela:
				radiance = intensity * kPi;
				break;
		};

		float dist = pow(saturate(1.0f - pow(r / radius, 4.0f)), 2.0f) / (r * r + 1.0f); //!< dist = func_win(r);

		return radiance * dist;
	}

	float3 GetColor(float r) {
		return color * GetRadiance(r);
	}

	float TraceShadow(RayDesc desc, RaytracingAccelerationStructure scene) {
		return shadow.TraceShadow(desc, scene);
	}
	
};
