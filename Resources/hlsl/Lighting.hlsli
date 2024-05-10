////////////////////////////////////////////////////////////////////////////////////////////
// lambert methods
////////////////////////////////////////////////////////////////////////////////////////////

float LambertReflection(float3 normal, float3 lightDirection) {
	float result;
	
	result = dot(normal, -lightDirection);
	
	return saturate(result);
}

float HalfLambertReflection(float3 normal, float3 lightDirection, float exponent = 2.0f) {
	float result;
	
	float NdotL = dot(normal, -lightDirection);
	result = pow(NdotL * 0.5f + 0.5f, exponent);
	
	return result;
}