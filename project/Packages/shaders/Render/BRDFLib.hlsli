#pragma once

/*
# reference
UE4 PBR: https://de45xmedrsdbp.cloudfront.net/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
         https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile //!< 2014 version.
*/

////////////////////////////////////////////////////////////////////////////////////////////
// const variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kPi = 3.14159265359f;

////////////////////////////////////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////////////////////////////////////

float3 DiffuseBRDF(float3 diffuse) {
	return diffuse / kPi;
}

//! @brief FresnelReflectance(Spherical Gaussian)
//! @param VdotH: saturate(dot(v, l))
//! @param f0: f0
float3 FresnelReflectance(float VdotH, float3 f0) {
	float exponent = (-5.55473f * VdotH - 6.98316f) * VdotH;
	return f0 + (1.0f - f0) * exp2(exponent);
}

//! @brief CalculateSpecularF(Schlick)
//! @param NdotV: saturate(dot(n, v))
//! @param NdotL: saturate(dot(n, h))
float GeometricAttenuation(float NdotV, float NdotL, float roughness) {
	const float a = roughness * roughness;
	const float k = a / 2.0f;

	float g1_v = NdotV / (NdotV * (1.0f - k) + k); //!< G1(v)
	float g1_l = NdotL / (NdotL * (1.0f - k) + k); //!< G1(l)

	return g1_v * g1_l;
}

//! @brief DistributionFunction(GGX/Trowbridge-Reitz)
//! @param NdotH: saturate(dot(n, h))
//! @param roughness: roughness
float DistributionFunction(float NdotH, float roughness) {
	const float a  = roughness * roughness;
	const float a2 = a * a;
	const float NdotH2 = NdotH * NdotH;

	return a2 / (kPi * pow(NdotH2 * (a2 - 1.0f) + 1.0f, 2.0f));
}

//! @brief SpecularBRDF
//! @param f: FresnelReflectance
//! @param g: GeometricAttenuation
//! @param d: DistributionFunction
//! @param NdotL: saturate(dot(n, l))
//! @param NdotV: saturate(dot(n, v))
float3 SpecularBRDF(float3 f, float g, float d, float NdotL, float NdotV) {
	return (f * g * d) / (4.0f * NdotL * NdotV);
}
