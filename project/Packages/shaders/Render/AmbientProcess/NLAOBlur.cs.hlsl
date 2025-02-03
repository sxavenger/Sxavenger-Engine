//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "AmbientProcess.hlsli"
#include "NLAO.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gMain : register(u0); //!< main texture

Texture2D<float4> gMaterial_AO : register(t0); //!< material and ao texture

////////////////////////////////////////////////////////////////////////////////////////////
// variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float pi_v = 3.14159265359f;

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessOutput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ProcessOutput {
	
	//* member *//
	
	float ao;
	
	//* method *//
	
	void SetOutput(uint2 index) {
		float4 color = gMain[index];
		color.rgb *= float3(ao, ao, ao);
		gMain[index] = color;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// common method
////////////////////////////////////////////////////////////////////////////////////////////

bool CheckOverTexture(uint2 index) {
	return any(index >= gConfig.size);
}

float Gaussian2d(float2 diff, float sigma) {
	float a = 1.0f / (2.0f * pi_v * sigma * sigma);
	float b = exp(-(diff.x * diff.x + diff.y * diff.y) / (2.0f * sigma * sigma));
	return a * b;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}
	
	ProcessOutput output = (ProcessOutput)0;
	
	static const int2 kScale = int2(gParameter.size);
	
	float sum = 0.0f;
	float weightSum = 0.0f;
	
	for (int x = -kScale.x; x <= kScale.x; ++x) {
		for (int y = -kScale.y; y <= kScale.y; ++y) {

			int2 sample_index = int2(index) + int2(x, y);
			
			float weight = Gaussian2d(float2(x, y), 2.0f);
			
			sum       += weight * gMaterial_AO[sample_index].a;
			weightSum += weight;
		}
	}
	
	output.ao = saturate(sum / weightSum);
	output.SetOutput(index);
	
}
