#pragma once

/*
# reference
UE4 PBR: https://de45xmedrsdbp.cloudfront.net/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
         https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile //!< 2014 version.
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Math.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////////////////////////////////////

float3 DiffuseBRDF(float3 diffuse) {
	return diffuse / kPi;
}

//! @brief FresnelReflectance(Spherical Gaussian)
//! @param VdotH: saturate(dot(v, l))
//! @param f0: f0
float3 F_SphericalGaussian(float VdotH, float3 f0) {
	VdotH = saturate(VdotH + kEpsilon);
	
	float exponent = (-5.55473f * VdotH - 6.98316f) * VdotH;
	return f0 + (1.0f - f0) * exp2(exponent);
}

//! @brief GeometricAttenuation(Schlick)
//! @param NdotV: saturate(dot(n, v))
//! @param NdotL: saturate(dot(n, h))
float G_Schlick(float NdotV, float NdotL, float roughness) {
	NdotV = saturate(NdotV + kEpsilon);
	NdotL = saturate(NdotL + kEpsilon);
	
	const float a = roughness * roughness;
	const float k = a * 0.5f;

	float g1_v = NdotV / (NdotV * (1.0f - k) + k); //!< G1(v)
	float g1_l = NdotL / (NdotL * (1.0f - k) + k); //!< G1(l)

	return g1_v * g1_l;
}

//! @brief Visibility[GeometricTerm](Height-Correlated)
float V_HeightCorrelated(float NdotV, float NdotL, float roughness) {
	const float a  = roughness * roughness;
	const float a2 = a * a;

	NdotV = saturate(NdotV + kEpsilon);
	NdotL = saturate(NdotL + kEpsilon);

	float lamda_v = 1.0f / (NdotV + sqrt(a2 + (1.0f - a2) * (NdotV * NdotV)));
	float lamda_l = 1.0f / (NdotL + sqrt(a2 + (1.0f - a2) * (NdotL * NdotL)));

	return lamda_l * lamda_v;
}

//! @brief DistributionFunction(GGX/Trowbridge-Reitz)
//! @param NdotH: saturate(dot(n, h))
//! @param roughness: roughness
float D_GGX(float NdotH, float roughness) {
	NdotH = saturate(NdotH + kEpsilon);
	
	const float a  = roughness * roughness;
	const float a2 = a * a;

	float domi = (NdotH * NdotH) * (a2 - 1.0f) + 1.0f;

	return a2 / (kPi * domi * domi);
}

//! @brief SpecularBRDF
//! @param f: FresnelReflectance
//! @param g: GeometricAttenuation
//! @param d: DistributionFunction
//! @param NdotL: saturate(dot(n, l))
//! @param NdotV: saturate(dot(n, v))
float3 SpecularBRDF(float3 f, float g, float d, float NdotL, float NdotV) {
	NdotL = saturate(NdotL + kEpsilon);
	NdotV = saturate(NdotV + kEpsilon);
	
	return (f * g * d) / (4.0f * NdotL * NdotV);
}

//! @brief SpecularBRDF
//! @param f: FresnelReflectance
//! @param v: Visibility
//! @param d: DistributionFunction
float3 SpecularBRDF(float3 f, float v, float d) {
	return f * v * d;
}
