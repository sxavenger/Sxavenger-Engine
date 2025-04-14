//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"
#include "../../Hammersley.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

SamplerState gSampler : register(s0);

struct Parameter {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint type;
	//!< 0: none, 1: texture cube, 2: texture 2d
	
	uint index;
	
	//=========================================================================================
	// public methods
	//=========================================================================================

	float3 SampleColor(float3 v) {
		switch (type) {
			case 1:
				{
					TextureCube<float4> texture = ResourceDescriptorHeap[index];
					return texture.SampleLevel(gSampler, v).rgb;
				}
				break;

			case 2:
				{
					Texture2D<float4> texture = ResourceDescriptorHeap[index];
					float theta = acos(clamp(v.y, -1.0f, 1.0f));
					float p = atan2(v.z, v.x);
					float phi = (p < 0.0f) ? p + kTau : p;

					float2 uv = float2(saturate(phi * kTauInv), saturate(theta * kPiInv));
					return texture.Sample(gSampler, uv).rgb;
				}
				break;
		}

		return float3(0.0f, 0.0f, 0.0f);
	}
	
};
ConstantBuffer<Parameter> gParameter : register(b0);

Texture2D<float4> gBRDFLut : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

//float3 CalculateIrradiance() {
//}

float3 ImportanceSampleGGX(float2 xi, float roughness, float3 n) {
	float a = roughness * roughness;
	
	float phi = 2.0f * kPi * xi.x;
	float cosTheta = sqrt((1.0f - xi.y) / (1.0f + (a * a - 1.0f) * xi.y));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	float3 h = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

	float3 up = abs(n.z) < 0.999 ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
	float3 tangentX = normalize(cross(up, n));
	float3 tangentY = cross(n, tangentX);

	return tangentX * h.x + tangentY * h.y + n * h.z;
}

float3 PrefilterEnviromentMap(float3 r, float roughness) {

	static const uint kNumSamples = 32;

	float3 n = r;
	float3 v = r;

	float3 color = 0.0f;
	float total = 0.0f;
	
	for (uint i = 0; i < kNumSamples; ++i) {
		float2 xi = Hammersley(i, kNumSamples);
		float3 h  = ImportanceSampleGGX(xi, roughness, n);
		float3 l  = 2.0f * dot(v, h) * h - v;

		float NdotL = saturate(dot(n, l));

		if (NdotL > 0.0f) {
			color += gParameter.SampleColor(l) * NdotL;
			total += NdotL;
		}
	}

	return color / total;
}

float2 IntegrateBRDF(float roughness, float NdotV) {
	
	static const uint kNumSamples = 64;
	
	float3 v = float3(sqrt(1.0f - NdotV * NdotV), 0.0f, NdotV);

	float a = 0.0f;
	float b = 0.0f;

	for (uint i = 0; i < kNumSamples; ++i) {
		float2 xi = Hammersley(i, kNumSamples);
		float3 h  = ImportanceSampleGGX(xi, roughness, v);
		float3 l  = 2.0f * dot(v, h) * h - v;

		float NdotL = saturate(l.z);
		float NdotH = saturate(h.z);
		float VdotH = saturate(dot(v, h));

		if (NdotL > 0.0f) {
			float g     = GeometricAttenuation(NdotV, NdotL, roughness);
			float g_vis = g * VdotH / (NdotV * NdotH);
			float fc    = pow(1.0f - VdotH, 5.0f);

			a += g_vis * (1.0f - fc);
			b += fc * g_vis;
		}
	}

	return float2(a, b) / kNumSamples;
}

float3 ApproximateSpecularIBL(float3 specularColor, float roughness, float3 n, float3 v) {
	float NdotV = saturate(dot(n, v));
	
	const float2 brdf = gBRDFLut.Sample(gSampler, float2(NdotV, 1.0f - roughness)).rg;

	float3 r = -normalize(reflect(v, n));

	//* diffuse


	//* specular
	float3 specularIrradiance = PrefilterEnviromentMap(r, roughness);
	float3 specular = specularIrradiance * (specularColor * brdf.r + brdf.g);

	return specular;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);

	//* Cameraの情報を取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);
	float3 r = reflect(-v, surface.normal);

	// f0
	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0
	
	// specular Albedo
	//!< 金属(metallic = 1.0f) -> f0
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 specularAlbedo = lerp(f0, surface.albedo, surface.metallic);

	output.color.rgb = ApproximateSpecularIBL(specularAlbedo, surface.roughness, surface.normal, v);

	output.color.a = 1.0f;
	return output;
};
