//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Denoiser.hlsli"
#include "../DeferredBufferIndex.hlsli"
#include "../../Library/Math.hlsli"
#include "../../Library/ACES.hlsli"

//=========================================================================================
// define
//=========================================================================================

#define _NUM_THREAD_X 16
#define _NUM_THREAD_Y 16

//=========================================================================================
// buffer
//=========================================================================================

//* output texture
RWTexture2D<float4> gOutput : register(u0);

cbuffer Parameter : register(b1) { //!< test
	float sigma_n;
	float sigma_l;
	float sigma_s;
};

//* input texture
Texture2D<float4> gReservoirDiffuse  : register(t0);
Texture2D<float4> gReservoirSpecular : register(t1);
Texture2D<uint3> gMoment             : register(t2);
SamplerState gSampler                : register(s0);

ConstantBuffer<DeferredBufferIndexConstantBuffer> gDeferredBufferIndex : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// Config cbuffer 32bitconstants.
////////////////////////////////////////////////////////////////////////////////////////////
cbuffer Config : register(b2, space1) {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	uint maxSampleCount;
	uint samplesPerFrame;
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float4 GetIndirectReservoir(uint2 index) {
	return gReservoirDiffuse[index] + gReservoirSpecular[index];
}

float4 SampleIndirectReservoir(float2 uv, float lod) {
	return gReservoirDiffuse.SampleLevel(gSampler, uv, lod) + gReservoirSpecular.SampleLevel(gSampler, uv, lod);
}

float4 SampleIndirectReservoir(float2 uv, float lod_diffuse, float lod_specular) {
	return gReservoirDiffuse.SampleLevel(gSampler, uv, lod_diffuse) + gReservoirSpecular.SampleLevel(gSampler, uv, lod_specular);

}

float CalculateExpDepthWeight(float p, float q) {
	const float sigma_z = 0.1f;
	return -abs(p - q) / (sigma_z + kEpsilon);
}

float CalculateExpPositionWeight(float3 p, float3 q) {
	return -abs(dot(p - q, p - q)) / (sigma_s + kEpsilon);
}

float CalculateNormalWeight(float3 n, float3 m) {
	return pow(max(0.0f, dot(n, m)), sigma_n);
}

// ガウシアンカーネルを求める関数
float Gaussian2D(int2 x, float sigma) {
	float twoSigma2 = 2.0 * sigma * sigma;
	float coeff = 1.0 / (kPi * twoSigma2);
	return coeff * exp(-(x.x * x.x + x.y * x.y) / twoSigma2);
}

float CalculateExpLuminanceWeight(float3 p, float3 q, float v) {
	float l_p = dot(p, ACES::AP1_RGB2Y);
	float l_q = dot(q, ACES::AP1_RGB2Y);

	return -abs(l_p - l_q) / (sigma_l * v * l_p + kEpsilon);
}

float3 CalculateLuminanceRGB(float3 color) {
	return float3(
		dot(float3(color.r, 0.0f, 0.0f), ACES::AP1_RGB2Y),
		dot(float3(0.0f, color.g, 0.0f), ACES::AP1_RGB2Y),
		dot(float3(0.0f, 0.0f, color.b), ACES::AP1_RGB2Y)
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREAD_X, _NUM_THREAD_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;

	if (CheckOutOfRange(index)) {
		return;
	}

	DeferredSurface surface;
	if (!surface.GetSurface(gDeferredBufferIndex.Get(), index)) {
		gOutput[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return;
	}

	float weight    = 1.0f;
	float3 variance = SampleIndirectReservoir(float2(index) / float2(size), 0.0f).rgb;

	//!< A-Trousを採用
	
	static const uint kRecursionCount = 4;

	for (uint i = 0; i < kRecursionCount; ++i) {
		
		int2 offsets[] = {
			int2(-1,  0), int2(0, -1), int2(1,  0), int2(0, 1),
			int2(-1, -1), int2(1, -1), int2(-1, 1), int2(1, 1)
		};

		for (uint j = 0; j < 8; ++j) {

			int2 sample_pos = int2(index) + offsets[j] * (1u << i);

			if (CheckOutOfRange(sample_pos)) {
				continue;
			}

			DeferredSurface sample_surface;
			if (!sample_surface.GetSurface(gDeferredBufferIndex.Get(), sample_pos)) {
				continue;
			}

			float exp_w = 0.0f;
			exp_w += CalculateExpDepthWeight(surface.depth, sample_surface.depth);                  //!< 深度
			exp_w += CalculateExpPositionWeight(surface.position.xyz, sample_surface.position.xyz); //!< 空間情報

			float w = exp(exp_w);
			w *= CalculateNormalWeight(surface.normal, sample_surface.normal); //!< 法線
			//w *= Gaussian2D(offsets[j] * i, 1.0f); //!< ガウシアン

			float2 uv = float2(sample_pos) / float2(size);

			float2 lod = float2(
				(1.0f - float(gMoment[sample_pos].y) / maxSampleCount) * i / float(kRecursionCount - 1.0f) * 6.0f,
				(1.0f - float(gMoment[sample_pos].z) / maxSampleCount) * i / float(kRecursionCount - 1.0f) * 6.0f
			);
			
			variance += SampleIndirectReservoir(uv, 0.0f).rgb * w;
			weight   += w;
		}
	}

	variance /= weight;

	gOutput[index].rgb = variance;
	gOutput[index].a   = 1.0f;
}
