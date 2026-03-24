//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Bloom.hlsli"

//* library
#include "../../../../Library/ACES.hlsli"
#include "../../../../Library/Mathmatic.hlsli"
#include "../../../../Library/RandomLib.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput     : register(t0);
Texture2D<float4> gLuminance : register(t1);

SamplerState gSampler : register(s0);	

RWTexture2D<float4> gOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float Gaussian2d(float2 x, float sigma) {
	float twoSigma2 = 2.0 * sigma * sigma;
	float coeff = 1.0 / (Mathmatic::kPi * twoSigma2);
	return coeff * exp(-(x.x * x.x + x.y * x.y) / twoSigma2);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void main(uint3 dispathThreadId : SV_DispatchThreadID) {

	uint2 index = dispathThreadId.xy;
	
	if (CheckOverDimension(index)) {
		return;
	}

	//* Octaweb sampling を採用

	static const uint kDensity   = 14; //!< 方向ごとのsampling数 <= 羽枚数としてparameter化する
	static const uint kCount     = 8; //!< sampling方向数 

	float weight_sum     = 1.0f;
	float4 luminance_sum = gInput[index] * gLuminance[index];

	float noise = 0.0f;

	if (gParameter.isStochastic) {
		noise = PseudoRandom(index) * 2.0f - 1.0f;
	}

	for (uint i = 1; i < kCount; ++i) {

		float rad = gParameter.radius * i;
		float cnt = max(kDensity * i, 1.0f);

		float inc = Mathmatic::kTau / cnt;

		float lod = (float(i) / float(kCount)) * 4.0f;

		for (float j = 0; j < cnt; ++j) {
			float theta   = j * inc;
			float2 offset = rad * float2(cos(theta), sin(theta));

			float2 uv    = (float2(index) + offset * noise) / float2(dimension);
			float weight = Gaussian2d(offset, rad);

			luminance_sum += gLuminance.SampleLevel(gSampler, uv, 0) * weight;
			weight_sum    += weight;
			
		}
	}

	luminance_sum /= weight_sum;

	gOutput[index] = luminance_sum * gParameter.intensity + gInput[index];
	
}
