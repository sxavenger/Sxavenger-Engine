//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"
#include "Bloom.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gInput  : register(u0);
RWTexture2D<float4> gOutput : register(u1);


////////////////////////////////////////////////////////////////////////////////////////////
// variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float pi_v = 3.14159265358979323846f;

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

float Gaussian2d(float2 diff, float sigma) {
	float a = 1.0f / (2.0f * pi_v * sigma * sigma);
	float b = exp(-(diff.x * diff.x + diff.y * diff.y) / (2.0f * sigma * sigma));
	return a * b;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessOutput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ProcessOutput {

	//* member *//
	
	float4 color;
	
	//* method *//
	
	void SetOutput(uint2 index) {
		gOutput[index] = color;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispathThreadId : SV_DispatchThreadID) {
	//* 輝度抽出 and blur

	uint2 index = dispathThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return;
	}
	
	ProcessOutput output = (ProcessOutput)0;
	
	static const int2 kSamples = int2(gParameter.size);
	
	float4 color_sum = 0.0f;
	float weight_sum = 0.0f;
	
	for (int x = -kSamples.x; x <= kSamples.x; ++x) {
		for (int y = -kSamples.y; y <= kSamples.y; ++y) {

			int2 sample_index = int2(index) + int2(x, y);
			
			float4 color = gInput[sample_index];
			
			// 閾値からの輝度抽出
			color.rgb = max(color.rgb - gParameter.threshold, 0.0f);
			
			float weight = Gaussian2d(float2(x, y), 12.0f);
			
			color_sum += color * weight;
			weight_sum += weight;
		}
	}
	
	output.color = color_sum / weight_sum;
	output.SetOutput(index);
	
}
