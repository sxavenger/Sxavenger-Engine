////////////////////////////////////////////////////////////////////////////////////////////
// Light structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Light {
	float4 color;
	float4 position;
	float3 direction;
	float intensity;
	float range;
	float decay;
	float angle;
	float falloffAngle;

	int lightType;
};

namespace LIGHT {
	static const int DIRECTIONAL = 0,
	                 POINT       = 1,
	                 SPOT        = 2;
}

////////////////////////////////////////////////////////////////////////////////////////////
// lambert methods
////////////////////////////////////////////////////////////////////////////////////////////

float LambertReflection(float3 normal, float3 directionToLight) {
	float result;
	
	result = dot(normal, directionToLight);
	
	return saturate(result);
}

float HalfLambertReflection(float3 normal, float3 directionToLight, float exponent = 2.0f) {
	float result;
	
	float NdotL = dot(normal, directionToLight);
	result = pow(NdotL * 0.5f + 0.5f, exponent);
	
	return result;
}

float BlinnPhongReflection(float3 worldPos, float3 normal, float3 directionToLight, float3 directionToCamera, float specPow) {
	
	float3 halfVector = normalize(directionToLight + directionToCamera);
	float NdotH = max(0, dot(normal, halfVector));
	float spec = pow(NdotH, specPow);
	
	return spec;
	
}

float PhongReflection(float3 worldPos, float3 normal, float3 directionToLight, float3 directionToCamera, float specPow) {
	
	float3 reflection = reflect(directionToLight, normal);
	float RdotE = dot(reflection, directionToCamera);
	float spec = pow(saturate(RdotE), specPow);

	return spec;
}