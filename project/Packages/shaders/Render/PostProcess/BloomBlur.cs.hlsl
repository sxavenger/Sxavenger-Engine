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
	//* blur

	uint2 index = dispathThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return;
	}
	
	ProcessOutput output = (ProcessOutput)0;
	
	float4 color_sum = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float weight_sum = 0.0f;
	
	static const int2 kSize = int2(gParameter.size);
	
	for (int y = -kSize.y; y <= kSize.y; ++y) {
		for (int x = -kSize.x; x <= kSize.x; ++x) {
			float2 diff = float2(x, y);
			float weight = Gaussian2d(diff, 12.0f);
			
			float4 color = gInput[index + int2(x, y)];
			
			color_sum  += color * weight;
			weight_sum += weight;
		}
	}
	
	color_sum /= weight_sum; //!< ³‹K‰»
	
	output.color = color_sum;
	output.SetOutput(index);

}
