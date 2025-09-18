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
// SpotLightComponent structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SpotLightComponent {
	
	//=========================================================================================
	// public variables
	//=========================================================================================

	InlineShadow shadow;

	float3 color;
	LightUnits::Type unit;
	float intensity;
	float radius;
	float2 coneAngle; // x: inner, y: outer
	//!< CONSTRAINT: 0 <= inner < outer <= pi/4
	
	//!< unreal engine is angle range [0 ~ 80]

	//=========================================================================================
	// public methods
	//=========================================================================================

	float3 GetDirectionFromSurface(float3 light_position, float3 surface_position) {
		return normalize(light_position - surface_position);
	}

	float GetLightMask(RaytracingAccelerationStructure scene, float3 light_position, float3 light_direction, float3 surface_position) {
		
		float distance = length(light_position - surface_position);
		float3 l       = GetDirectionFromSurface(light_position, surface_position);

		float attenuation_distance = Square(saturate(1.0f - Square(distance / radius))) / (Square(distance) + 1.0f);

		float cosInnerCone = cos(coneAngle.x);
		float cosOuterCone = cos(coneAngle.y);
		
		float attenuation_angle = Square(saturate((dot(l, -light_direction) - cosOuterCone) / (cosInnerCone - cosOuterCone)));

		static const float kTMin = 0.001f;
		static const float kTMax = 10000.0f;

		RayDesc desc;
		desc.Origin = surface_position;
		desc.Direction = l;
		desc.TMin = kTMin;
		desc.TMax = distance;
		float attenuation_shadow = shadow.TraceShadow(desc, scene);

		return attenuation_distance * attenuation_angle * attenuation_shadow;
		
	}

	float GetIntensity() {
		return intensity * kPi;
	}

	float GetRadiance() {
		float radiance = 0.0f;

		switch (unit) {
			case LightUnits::Lumen:
				radiance = GetIntensity() / (2.0f * kPi * (1.0f - cos(coneAngle.y)));
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
