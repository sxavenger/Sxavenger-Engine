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
// RectLightComponent structure
////////////////////////////////////////////////////////////////////////////////////////////
struct RectLightComponent {

	//=========================================================================================
	// public variables
	//=========================================================================================

	InlineShadow shadow;

	float3 color;
	LightUnits::Type unit;
	float intensity;
	float radius;
	float2 source;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float3 GetPositionFromSurface(float4x4 light_matrix, float3 surface_position) {

		//!< TODO: matrix scale is not support.
		float3 light_origin = Mathmatic::GetPosition(light_matrix);
		
		float3 up    = Mathmatic::GetUpDirection(light_matrix);
		float3 right = Mathmatic::GetRightDirection(light_matrix);

		float2 half_source = source * 0.5f;

		float2 local = float2(
			dot(surface_position - light_origin, right),
			dot(surface_position - light_origin, up)
		);

		local = clamp(local, -half_source, half_source);

		return light_origin + right * local.x + up * local.y;
	}

	float3 GetDirectionFromSurface(float4x4 light_matrix, float3 surface_position) {
		float3 light_position = GetPositionFromSurface(light_matrix, surface_position);
		return normalize(light_position - surface_position);
	}

	float GetLightMask(RaytracingAccelerationStructure scene, float4x4 light_matrix, float3 surface_position) {

		float3 light_position = GetPositionFromSurface(light_matrix, surface_position) + Mathmatic::kEpsilon;

		float d  = length(light_position - surface_position);
		float3 l = normalize(light_position - surface_position);

		float attenuation_distance = Mathmatic::Square(saturate(1.0f - Mathmatic::Square(d / radius))) / (Mathmatic::Square(d) + 1.0f);

		float attenuation_angle = dot(l, -Mathmatic::GetForwardDirection(light_matrix));

		if (attenuation_distance * attenuation_angle <= 0.0f) {
			return 0.0f;
		}

		static const float kTMin = 0.001f;
		static const float kTMax = 10000.0f;

		RayDesc desc;
		desc.Origin    = surface_position;
		desc.Direction = l;
		desc.TMin      = kTMin;
		desc.TMax      = d;
		
		float attenuation_shadow = shadow.TraceShadow(desc, scene);
		
		return attenuation_distance * attenuation_angle * attenuation_shadow;

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
