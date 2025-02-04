#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

float CalculateDiffuseLambert(float3 n, float3 l) {
	return saturate(dot(n, l));
}

float CalculateDiffuseHalfLambert(float3 n, float3 l) {
	return saturate(dot(n, l) * 0.5f + 0.5f);
}

float CalculateSpecularBlinnPhong(float3 n, float3 l, float3 v, float power = 64.0f) {
	float3 h = normalize(l + v);
	return pow(saturate(dot(n, h)), power);
}

