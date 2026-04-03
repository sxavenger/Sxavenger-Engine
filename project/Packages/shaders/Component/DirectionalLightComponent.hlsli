#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "LightComponentCommon.hlsli"

//* library
#include "../Library/Mathmatic.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLightComponent structure
////////////////////////////////////////////////////////////////////////////////////////////
struct DirectionalLightComponent {

	//=========================================================================================
	// public variables
	//=========================================================================================

	InlineShadow shadow;

	float3 color;
	float intensity; //!< [lux]

	//=========================================================================================
	// public methods
	//=========================================================================================

	float3 GetDirectionFromSurface(float3 light_direction) {
		return -light_direction;
	}

	float GetLightMask(RaytracingAccelerationStructure scene, float3 light_direction, float3 surface_position) {

		float3 l = GetDirectionFromSurface(light_direction);
		
		float attenuation_shadow = shadow.TraceShadow(scene, surface_position, l);
		
		return attenuation_shadow;
	}

	float GetIntensity() {
		return intensity * Mathmatic::kPi;
	}

	float GetRadiance() {
		return GetIntensity();
	}

	float3 GetColorMask() {
		return color * GetRadiance();
	}
	
};
