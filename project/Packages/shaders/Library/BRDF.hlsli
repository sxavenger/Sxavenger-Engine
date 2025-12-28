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
 https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing
*/

#ifndef BRDF_LIBRARY
#define BRDF_LIBRARY

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../Library/Math.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// BxDFContext structure
////////////////////////////////////////////////////////////////////////////////////////////
struct BxDFContext {

	//=========================================================================================
	// public variables
	//=========================================================================================

	half NdotV;
	half NdotL;
	half NdotH;
	half VdotH;

	//=========================================================================================
	// public methods
	//=========================================================================================

	static BxDFContext Create(float3 n, float3 v, float3 l) {

		BxDFContext context;

		float3 h = normalize(l + v); //!< half vector

		context.NdotV = saturate(dot(n, v));
		context.NdotL = saturate(dot(n, l));
		context.NdotH = saturate(dot(n, h));
		context.VdotH = saturate(dot(v, h));

		return context;

		// TODO: Epsilon対策
	}

	bool IsValid() {
		return NdotL > 0.0h;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// BxDFAlbedo structure
////////////////////////////////////////////////////////////////////////////////////////////
struct BxDFAlbedo {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float3 diffuse;
	float3 specular;

	//=========================================================================================
	// public methods
	//=========================================================================================

	static BxDFAlbedo Create(float3 albedo, float metallic) {

		BxDFAlbedo bxdf;

		static const float3 kMinFrenel = float3(0.04f, 0.04f, 0.04f); //!< 非金属の最小Frenel値

		bxdf.diffuse  = albedo * (1.0f - kMinFrenel) * (1.0f - metallic);
		bxdf.specular = lerp(kMinFrenel, albedo, metallic);

		return bxdf;
		
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 Diffuse_Lambert(BxDFAlbedo albedo) {
	return albedo.diffuse * (1.0f / kPi);
}

//! @brief FresnelReflectance(Spherical Gaussian)
//! @param context: BxDFContext
//! @param albedo: BxDFAlbedo
float3 F_SphericalGaussian(BxDFContext context, BxDFAlbedo albedo) {
	float exponent = (-5.55473f * context.VdotH - 6.98316f) * context.VdotH;
	return albedo.specular + (1.0f - albedo.specular) * exp2(exponent);
}

//! @brief GeometricAttenuation(Schlick)
//! @param context: BxDFContext
//! @param roughness: surface - roughness
float G_Schlick(BxDFContext context, float roughness) {
	const float a = roughness * roughness;
	const float k = a * 0.5f;

	float g1_v = context.NdotV * rcp(context.NdotV * (1.0f - k) + k); //!< G1(v)
	float g1_l = context.NdotL * rcp(context.NdotL * (1.0f - k) + k); //!< G1(l)

	return g1_v * g1_l;
}

//! @brief Visibility[GeometricTerm](Height-Correlated)
//! @param context: BxDFContext
//! @param roughness: surface - roughness
float V_HeightCorrelated(BxDFContext context, float roughness) {
	const float a  = roughness * roughness;
	const float a2 = a * a;

	float lamda_v = context.NdotL * sqrt(context.NdotV * (context.NdotV - context.NdotV * a2) + a2);
	float lamda_l = context.NdotV * sqrt(context.NdotL * (context.NdotL - context.NdotL * a2) + a2);
	
	return 0.5f / (lamda_v + lamda_l);
}

//! @brief DistributionFunction(GGX/Trowbridge-Reitz)
//! @param context: BxDFContext
//! @param roughness: roughness
float D_GGX(BxDFContext context, float roughness) {
	const float a  = roughness * roughness;
	const float a2 = a * a;

	float f = (context.NdotH * a2 - context.NdotH) * context.NdotH + 1.0f;

	return a2 / (kPi * f * f);
}

//! @brief SpecularBRDF
//! @param f: FresnelReflectance
//! @param g: GeometricAttenuation
//! @param d: DistributionFunction
//! @param context: BxDFContext
float3 Specular_BRDF(float3 f, float g, float d, BxDFContext context) {
	return (f * g * d) / (4.0f * context.NdotL * context.NdotV);
}

//! @brief SpecularBRDF
//! @param f: FresnelReflectance
//! @param v: Visibility
//! @param d: DistributionFunction
float3 Specular_BRDF(float3 f, float v, float d) {
	return f * v * d;
}

//! @brief EvaluateBRDF
//! @param albedo: BxDFAlbedo
//! @param context: BxDFContext
//! @param roughness: surface - roughness
float3 EvaluateBRDF(BxDFAlbedo albedo, BxDFContext context, float roughness) {
	
	float3 f = F_SphericalGaussian(context, albedo);
	float v  = V_HeightCorrelated(context, roughness);
	float d  = D_GGX(context, roughness);
	
	return Diffuse_Lambert(albedo) + Specular_BRDF(f, v, d);
}

#endif
