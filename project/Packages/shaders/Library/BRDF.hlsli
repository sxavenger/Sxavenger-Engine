#pragma once

/*
# reference
> UE4 PBR:
 https://de45xmedrsdbp.cloudfront.net/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
 https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile //!< 2014 version.

> PBR:
 https://google.github.io/filament/Filament.md.html#materialsystem/diffusebrdf

> Height-Correlated:
 https://light11.hatenadiary.com/entry/2020/03/03/195249#Height-Correlated-Smith%E9%96%A2%E6%95%B0
 https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg)
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
	VdotH = max(VdotH, kEpsilon);
	
	float exponent = (-5.55473f * VdotH - 6.98316f) * VdotH;
	return f0 + (1.0f - f0) * exp2(exponent);
}

//! @brief GeometricAttenuation(Schlick)
//! @param NdotV: saturate(dot(n, v))
//! @param NdotL: saturate(dot(n, h))
float G_Schlick(float NdotV, float NdotL, float roughness) {
	NdotV = max(NdotV, kEpsilon);
	NdotL = max(NdotL, kEpsilon);
	
	const float a = roughness * roughness;
	const float k = a * 0.5f;

	float g1_v = NdotV / (NdotV * (1.0f - k) + k); //!< G1(v)
	float g1_l = NdotL / (NdotL * (1.0f - k) + k); //!< G1(l)

	return g1_v * g1_l;
}

//! @brief Visibility[GeometricTerm](Height-Correlated)
float V_HeightCorrelated(float NdotV, float NdotL, float roughness) {
	NdotV = max(NdotV, kEpsilon);
	NdotL = max(NdotL, kEpsilon);

	const float a  = roughness * roughness;
	const float a2 = a * a;

	//float lamda_v = NdotL * sqrt(NdotV * NdotV * (1.0f - a2) + a2);
	//float lamda_l = NdotV * sqrt(NdotL * NdotL * (1.0f - a2) + a2);

	float lamda_v = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);
	float lamda_l = NdotL * sqrt((-NdotV * a2 + NdotV) * NdotV + a2);

	return 0.5f / (lamda_v + lamda_l);

}

//! @brief DistributionFunction(GGX/Trowbridge-Reitz)
//! @param NdotH: saturate(dot(n, h))
//! @param roughness: roughness
float D_GGX(float NdotH, float roughness) {
	NdotH = max(NdotH, kEpsilon);
	
	const float a  = roughness * roughness;
	const float a2 = a * a;

	float f = (NdotH * a2 - NdotH) * NdotH + 1.0f;

	return a2 / (kPi * f * f);
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
