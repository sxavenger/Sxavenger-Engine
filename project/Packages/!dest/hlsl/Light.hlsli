#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

float LambertReflection(float3 normal, float3 toLight) {

	float result = 0.0f;
	
	result = dot(normal, toLight);
	return saturate(result);
}

float HalfLambertReflection(float3 normal, float3 toLight, float exponent = 2.0f) {

	float result = 0.0f;
	float NdotL  = dot(normal, toLight);
	
	result = pow(NdotL * 0.5f + 0.5f, exponent);
	return result;
}

float BlinnPhongReflection(float3 worldPos, float3 normal, float3 toLight, float3 toCamera, float specPow) {
	float3 halfVector = normalize(toCamera + toLight);
	float NdotH       = saturate(dot(normal, halfVector));
	float spec        = pow(NdotH, specPow);
	return spec;
}

float PhongReflection(float3 worldPos, float3 normal, float3 toLight, float3 toCamera, float specPow) {
	float3 reflection = reflect(toLight, normal);
	float RdotE       = dot(reflection, toCamera);
	float spec        = pow(saturate(RdotE), specPow);
	return spec;
}

////////////////////////////////////////////////////////////////////////////////////////////
// LightType enum
////////////////////////////////////////////////////////////////////////////////////////////

//* enum typedef
typedef uint LIGHT_TYPE_;

//* enum namespace
namespace LIGHT_TYPE {
	static const uint NONE        = 0,
	                  DIRECTIONAL = 1,
	                  POINT       = 2,
	                  SPOT        = 3;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Light structure
////////////////////////////////////////////////////////////////////////////////////////////

struct Light {
	float4 colorAndIntencity; //!< rgb: color, a: intencity
	float4 position;
	float3 direction;
	float range;
	float dacay;
	float angle;
	float falloffAngle;
	LIGHT_TYPE_ type;
};

struct GlobalLightConfig {
	float ambient;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Light methods
////////////////////////////////////////////////////////////////////////////////////////////