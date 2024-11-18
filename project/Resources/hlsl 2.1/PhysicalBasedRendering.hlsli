#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "MathLib.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// PBRMaterial structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PBRMaterial {
	float roughness;
	float metallic;
};

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

//! @brief schlick fresnel (F)
float3 SchlickFresnel(float3 specular, float3 NdotL) {
	return specular + (1.0f - specular) * pow((1.0f - NdotL), 5.0f);
}

//! @brief beckmann (D)
float D_Beckmann(float m, float NH) {
	
	float c2 = NH * NH;
	float c4 = c2 * c2;
	float m2 = m * m;

	return (1.0f / (m2 * c4)) * exp((-1.0f / m2) * (1.0f - c2) / c2);
}

//! @brief GGX (D)
float D_GGX(float m2, float NdotH) {
	float f = (NdotH * m2 - NdotH) * NdotH + 1.0f;
	return m2 / (pi_v * f * f);
}

//! @brief v-cavity (G2)
float G2_Vcavity(float NdotH, float NdotV, float NdotL, float VdotH) {
	return min(1.0f, min(2.0f * NdotH * NdotV / VdotH, 2.0f * NdotH * NdotL / VdotH));
}

float G2_Smith(float NdotL, float NdotV, float m2) {
	float NdotL2 = NdotL * NdotL;
	float NdotV2 = NdotV * NdotV;

	float Lambda_V = (-1.0f + sqrt(m2 * (1.0f - NdotV2) / NdotV2 + 1.0f)) * 0.5f;
	float Lambda_L = (-1.0f + sqrt(m2 * (1.0f - NdotL2) / NdotL2 + 1.0f)) * 0.5f;
	return 1.0f / (1.0f + Lambda_L + Lambda_V);
}

//! @brief diffuse
float3 CalculateDiffuse(PBRMaterial material, float3 baseColor = float3(1.0f, 1.0f, 1.0f)) {
	float3 kd = baseColor * (1.0f - material.metallic);
	return kd * (1.0f / pi_v);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PBR model methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 CookTorrance(PBRMaterial material, float3 normal, float3 toCameraDir, float3 toLightDir, float3 baseColor = float3(1.0f, 1.0f, 1.0f)) {

	float3 R = normalize(-toCameraDir + 2.0f * dot(normal, toCameraDir) * normal);
	float3 H = normalize(toCameraDir + toLightDir);

	float NdotH = saturate(dot(normal, H));
	float NdotV = saturate(dot(normal, toCameraDir));
	float NdotL = saturate(dot(normal, toLightDir));
	float VdotH = saturate(dot(toCameraDir, H));

	float3 ks = baseColor * material.metallic;
	float a   = material.roughness * material.roughness;
	
	float d   = D_Beckmann(a, NdotH);
	float g2  = G2_Vcavity(NdotH, NdotV, NdotL, VdotH);
	float3 fr = SchlickFresnel(ks, NdotL);

	float3 specular = (d * g2 * fr) / (4.0f * NdotV * NdotL);
	float3 diffuse  = CalculateDiffuse(material, baseColor);

	return (diffuse + specular) * NdotL;
	
}

float3 GGX(PBRMaterial material, float3 normal, float3 toCameraDir, float3 toLightDir, float3 baseColor = float3(1.0f, 1.0f, 1.0f)) {

	float3 H = normalize(toCameraDir + toLightDir);

	float NdotV = saturate(dot(normal, toCameraDir));
	float NdotH = saturate(dot(normal, H));
	float NdotL = saturate(dot(normal, toLightDir));
	float VdotH = saturate(dot(toCameraDir, H));

	float3 ks = baseColor * material.metallic;
	float a   = material.roughness * material.roughness;
	float m2  = a * a;
	
	float d   = D_GGX(m2, NdotH);
	float g2  = G2_Smith(NdotL, NdotV, m2);
	float3 fr = SchlickFresnel(ks, NdotL);

	float3 specular = (d * g2 * fr) / (4.0f * NdotV * NdotL);
	float3 diffuse  = CalculateDiffuse(material, baseColor);

	return (diffuse + specular) * NdotL;
}

