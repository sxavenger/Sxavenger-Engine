/*
# reference
 - https://dev.epicgames.com/documentation/en-us/unreal-engine/bloom-in-unreal-engine
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"
#include "../../Library/Gaussian.hlsli"
#include "../../Library/Blend.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput : register(t0);
SamplerState gSampler    : register(s0);

RWTexture2D<float4> gOutput : register(u0);

struct Parameter {
	float intensity;
	float threshold;
};
ConstantBuffer<Parameter> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispathThreadId : SV_DispatchThreadID) {

	uint2 index = dispathThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return;
	}

	static const int2 kSampleSize = int2(4, 4);
	//!< todo: 円形サンプリングに変更

	static const float kSigma = 1.0f;

	float4 color_sum = float4(0, 0, 0, 0);
	float weight_sum = 0.0f;

	for (int x = -kSampleSize.x; x <= kSampleSize.x; ++x) {
		for (int y = -kSampleSize.y; y <= kSampleSize.y; ++y) {

			int2 sampleIndex = index.xy + int2(x, y);
			float2 uv        = float2(sampleIndex) / float2(gConfig.size);

			float4 color = max(gInput.SampleLevel(gSampler, uv, 0.0f) - 1.0f, 0.0f);
			float weight = Gaussian2d(float2(x, y), kSigma);

			// todo: threshold

			color_sum  += color * weight;
			weight_sum += weight;
		}
	}

	float4 bloom = (color_sum / weight_sum);
	float4 scene = gInput.Load(uint3(index, 0));
	
	gOutput[index] = bloom + scene;
	
}
