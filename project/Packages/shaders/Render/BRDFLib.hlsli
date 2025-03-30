#pragma once

//* reference *//
//!< UE4 PBR: https://de45xmedrsdbp.cloudfront.net/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf

////////////////////////////////////////////////////////////////////////////////////////////
// const variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kPi = 3.14159265359f;

////////////////////////////////////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////////////////////////////////////

//* Diffuse *//
float3 CalculateDiffuseBRDF(float3 diffuse) {
	return diffuse / kPi;
}

//* Specular *//
//* f(l, v) = F(l, v) * G(l, v) * D(l, v) / 4 * dot(n, l) * dot(n, v)

// Fresnel Reflection(F)
float3 CalculateSpecularF(float3 v, float3 h, float3 f0) {
	float exponent = (-5.55473f * dot(v, h) - 6.98316f) * dot(v, h);
	return f0 + (1.0f - f0) * exp2(exponent);
}

// Microfacet Distribution(D)
float CalculateSpecularD(float3 n, float3 h, float roughness) {
	const float a  = roughness * roughness;
	const float a2 = a * a;
	const float NdotH  = saturate(dot(n, h));
	const float NdotH2 = NdotH * NdotH;

	return a2 / (kPi * pow(NdotH2 * (a2 - 1.0f) + 1.0f, 2.0f));
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


float3 CalculateSpecularBRDF(float3 n, float3 v, float3 l, float roughness, float3 f0) {
	float3 h = normalize(l + v);
	float3 f = CalculateSpecularF(v, h, f0);
	float d = CalculateSpecularD(n, h, roughness);
	float g = CalculateSpecularG(n, v, l, roughness);
	return (f * g * d) / max((4.0f * saturate(dot(n, l)) * saturate(dot(n, v))), 0.001f);
}

