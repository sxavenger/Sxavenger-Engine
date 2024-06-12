////////////////////////////////////////////////////////////////////////////////////////////
// Lighting methods
////////////////////////////////////////////////////////////////////////////////////////////

float HalfLambertReflection(float3 normal, float3 toLightDirection, float exponent = 2.0f) {

	float NdotL = dot(normal, toLightDirection);
	float result = pow(NdotL * 0.5f + 0.5f, exponent);
	
	return result;

}

float3 BlinnPhong(float3 worldPos, float3 normal, float3 toLightDirection, float specPow, float4 specColor = float4(1, 1, 1, 1)) {
	
	float3 halfVector = normalize(toLightDirection - WorldRayDirection());
	float NdotH = max(0, dot(normal, halfVector));
	float spec = pow(NdotH, specPow);
	
	return specColor.rgb * spec;
	
}