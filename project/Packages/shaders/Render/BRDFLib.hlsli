#pragma once

//* Reference *//
//!< UE4 PBR: https://de45xmedrsdbp.cloudfront.net/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf

////////////////////////////////////////////////////////////////////////////////////////////
// const variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float pi_v = 3.14159265359f;

////////////////////////////////////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////////////////////////////////////

//* Diffuse *//
float3 CalculateDiffuseBRDF(float3 albedo) {
	return albedo / pi_v;
}

//* Specular *//
//* f(l, v) = F(l, v) * G(l, v) * D(l, v) / 4 * dot(n, l) * dot(n, v)

// Fresnel Reflection(F)
float CalculateSpecularF(float3 v, float3 h) {
	static const float f0 = 0.04f; //!< need parameter...?

	float exponent = (-5.55473f * dot(v, h) - 6.98316f) * dot(v, h);
	return f0 + (1.0f - f0) * pow(2.0f, exponent);
}

// Microfacet Distribution(D)
float CalculateSpecularD(float3 n, float3 h, float roughness) {
	const float a  = roughness * roughness;
	const float a2 = a * a;
	const float NdotH  = saturate(dot(n, h));
	const float NdotH2 = NdotH * NdotH;

	return a2 / (pi_v * pow(NdotH2 * (a2 - 1.0f) + 1.0f, 2.0f));
}

// Geometric Attenuation(G)
float CalculateSpecularG(float3 n, float3 v, float3 l, float roughness) {
	const float k = pow(roughness + 1.0f, 2.0f) / 8.0f;

	float NdotV = saturate(dot(n, v));
	float NdotL = saturate(dot(n, l));

	float g1_v = NdotV / (NdotV * (1.0f - k) + k); //!< G1(v)
	float g1_l = NdotL / (NdotL * (1.0f - k) + k); //!< G1(l)

	return g1_v * g1_l;
}


float CalculateSpecularBRDF(float3 n, float3 v, float3 l, float roughness) {
	float3 h = normalize(l + v);
	float f = CalculateSpecularF(v, h);
	float d = CalculateSpecularD(n, h, roughness);
	float g = CalculateSpecularG(n, v, l, roughness);
	return f * g * d / 4.0f * saturate(dot(n, l)) * saturate(dot(n, v));
}

