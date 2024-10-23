#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// LightType namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace LightType {
	static const int LIGHT_NONE        = 0,
	                 LIGHT_DIRECTIONAL = 1,
	                 LIGHT_POINT       = 2,
	                 LIGHT_SPOT        = 3;
}

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float LambertReflection(float3 normal, float3 toLight) {
	float result;
	
	result = dot(normal, toLight);
	
	return saturate(result);
}

float HalfLambertReflection(float3 normal, float3 toLight, float exponent = 2.0f) {
	float result;
	
	float NdotL = dot(normal, toLight);
	result = pow(NdotL * 0.5f + 0.5f, exponent);
	
	return result;
}

float BlinnPhongReflection(float3 worldPos, float3 normal, float3 toLight, float3 toCamera, float specPow) {
	
	float3 halfVector = normalize(toCamera + toLight);
	float NdotH = saturate(dot(normal, halfVector));
	float spec = pow(NdotH, specPow);
	
	return spec;
	
}

float PhongReflection(float3 worldPos, float3 normal, float3 toLight, float3 toCamera, float specPow) {
	
	float3 reflection = reflect(toLight, normal);
	float RdotE = dot(reflection, toCamera);
	float spec = pow(saturate(RdotE), specPow);

	return spec;
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// Light structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Light {
	
	//* members *//
	
	float4 color;
	float4 position;
	float3 direction;
	float range;
	float decay;
	float angle;
	float falloffAngle;

	int type;
	
	//* methods *//
	
	float4 GetLambertColor(float3 normal, float3 worldPos, float4 albed) {
		
		float3 result = albed.rgb;
		
		switch (type) {
			case LightType::LIGHT_DIRECTIONAL: {
				
					result *= LambertReflection(normal, -direction) * color.rgb * color.a;

				}
				break;
		
			case LightType::LIGHT_POINT: {
			
					float3 toLightDirection = normalize(position.xyz - worldPos);
					float distance          = length(position.xyz - worldPos);
					float factor            = pow(saturate(-distance / range + 1.0f), decay);
			
					result *= LambertReflection(normal, toLightDirection) * color.rgb * color.a * factor;
				
				}
				break;
			
			case LightType::LIGHT_SPOT: {
				
					// angles
					float3 toLightDirection = normalize(position.xyz - worldPos);
					float directionDot      = dot(toLightDirection, direction);
					float falloffFactor     = saturate((directionDot - angle) / (1.0f - angle));
				
					// distances
					float distance          = length(position.xyz - worldPos);
					float attenuationFactor = pow(saturate(-distance / range + 1.0f), decay);
					
					// factor
					float factor = attenuationFactor * falloffFactor;
				
					result *= LambertReflection(normal, toLightDirection) * color.rgb * color.a * factor;
				
				}
				break;
		}
		
		return float4(result, albed.a);
	}
	
	float4 GetHalfLambertColor(float3 normal, float3 worldPos, float4 albed) {
		float3 result = albed.rgb;
		
		switch (type) {
			case LightType::LIGHT_DIRECTIONAL: {
				
					result *= HalfLambertReflection(normal, -direction) * color.rgb * color.a;

				}
				break;
		
			case LightType::LIGHT_POINT: {
			
					float3 toLightDirection = normalize(position.xyz - worldPos);
					float distance          = length(position.xyz - worldPos);
					float factor            = pow(saturate(-distance / range + 1.0f), decay);
			
					result *= HalfLambertReflection(normal, toLightDirection) * color.rgb * color.a * factor;
				
				}
				break;
			
			case LightType::LIGHT_SPOT: {
				
					// angles
					float3 toLightDirection = normalize(position.xyz - worldPos);
					float directionDot      = dot(toLightDirection, direction);
					float falloffFactor     = saturate((directionDot - angle) / (1.0f - angle));
				
					// distances
					float distance          = length(position.xyz - worldPos);
					float attenuationFactor = pow(saturate(-distance / range + 1.0f), decay);
					
					// factor
					float factor = attenuationFactor * falloffFactor;
				
					result *= HalfLambertReflection(normal, toLightDirection) * color.rgb * color.a * factor;
				
				}
				break;
		}
		
		return float4(result, albed.a);
	}
};

//=========================================================================================
// config
//=========================================================================================

static const uint kMaxLightNum = 8; //!< light.h / .cpp

