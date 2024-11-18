#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../PhysicalBasedRendering.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Raytracing PBR model methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 RaytracingCookTorrance(PBRMaterial material, float3 normal, float3 toCameraDir, float3 toLightDir, float3 reflectionColor, float3 baseColor = float3(1.0f, 1.0f, 1.0f)) {

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
	float3 finalReflection = lerp(specular, reflectionColor, fr);
	
	float3 diffuse  = CalculateDiffuse(material, baseColor);

	return (diffuse + finalReflection) * NdotL;

}

float3 RaytracingGGX(PBRMaterial material, float3 normal, float3 toCameraDir, float3 toLightDir, float3 reflectionColor, float3 baseColor = float3(1.0f, 1.0f, 1.0f)) {

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
	float3 finalReflection = lerp(specular, reflectionColor, fr); //!< FIXME
	
	float3 diffuse  = CalculateDiffuse(material, baseColor);

	return (diffuse + finalReflection) * NdotL;
}