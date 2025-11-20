#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "Math.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// sample methods
////////////////////////////////////////////////////////////////////////////////////////////

void TangentSpace(float3 n, out float3 t, out float3 b) {
	float s = (n.z >= 0.0f) ? 1.0f : -1.0f;
	float a = -1.0f / (s + n.z);
	float bv = n.x * n.y * a;

	t = float3(1.0f + s * n.x * n.x * a, s * bv, -s * n.x);
	b = float3(bv, s + n.y * n.y * a, -n.y);
}

float3 ImportanceSampleLambert(float2 xi, float3 n) {

	float r = sqrt(xi.x);
	float phi = kTau * xi.y;

	float3 h;
	h.x = r * cos(phi);
	h.y = r * sin(phi);
	h.z = sqrt(1.0f - xi.x);

	float3 tangentX, tangentY;
	TangentSpace(n, tangentX, tangentY);

	return normalize(h.x * tangentX + h.y * tangentY + h.z * n);
}

float ImportanceSampleLambertPDF(float3 wi, float3 n) {
	return max(dot(wi, n), 0) * rcp(kPi);
}

float3 ImportanceSampleGGX(float2 xi, float roughness, float3 n) {
	float a = roughness * roughness;

	float phi = kTau * xi.x;
	float cosTheta = sqrt((1.0f - xi.y) / max(1.0f + (a * a - 1.0f) * xi.y, kEpsilon));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	float3 h; //!< 半径1の円周上にサンプリング
	h.x = sinTheta * cos(phi);
	h.y = sinTheta * sin(phi);
	h.z = cosTheta;

	float3 tangentX, tangentY;
	TangentSpace(n, tangentX, tangentY);

	return normalize(h.x * tangentX + h.y * tangentY + h.z * n);
}
