#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "LightComponentCommon.hlsli"

//* library
#include "../Library/Math.hlsli"

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

		static const float kTMin = 0.001f;
		static const float kTMax = 10000.0f;

		RayDesc desc;
		desc.Origin    = surface_position;
		desc.Direction = l;
		desc.TMin      = kTMin;
		desc.TMax      = kTMax;
		
		float attenuation_shadow = shadow.TraceShadow(desc, scene);
		
		return attenuation_shadow;
	}

	float GetIntensity() {
		return intensity * kPi;
	}

	float GetRadiance() {
		return GetIntensity();
	}

	float3 GetColorMask() {
		return color * GetRadiance();
	}
	
};
