#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "LightComponentCommon.hlsli"

//* library
#include "../Library/Mathmatic.hlsli"
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

	static float3 GetDirectionFromSurface(float3 light_position, float3 surface_position) {
		return normalize(light_position - surface_position);
	}

	float GetLightMask(RaytracingAccelerationStructure scene, float3 light_position, float3 surface_position) {
		
		float d  = length(light_position - surface_position);
		float3 l = GetDirectionFromSurface(light_position, surface_position);
		
		float attenuation_distance = Mathmatic::Square(saturate(1.0f - Mathmatic::Square(d / radius))) / (Mathmatic::Square(d) + 1.0f);

		if (attenuation_distance <= 0.0f) {
			return 0.0f;
		}
		
		float attenuation_shadow = shadow.TraceShadow(scene, surface_position, l, d);
		
		return attenuation_distance * attenuation_shadow;
	}

	float GetIntensity() {
		return intensity * Mathmatic::kPi;
	}

	float GetRadiance() {
		float radiance = 0.0f;

		switch (unit) {
			case LightUnits::Lumen:
				radiance = GetIntensity() / (Mathmatic::kPi * 4.0f);
				break;

			case LightUnits::Candela:
				radiance = GetIntensity();
				break;
		};

		return radiance;
	}

	float3 GetColorMask() {
		return color * GetRadiance();
	}
	
};
