//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gOutput : register(u0);
Texture2D<float4> gInput    : register(t0);

SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint2 GetInputDimension() {
	uint width, height;
	gInput.GetDimensions(width, height);
	return uint2(width, height);
}

float CubicWeight(float x) {
	x = abs(x);
	
	if (x <= 1.0f) {
		return (1.5f * x - 2.5f) * x * x + 1.0f;
		
	} else if (x < 2.0f) {
		return ((-0.5f * x + 2.5f) * x - 4.0f) * x + 2.0f;
	}

	return 0.0f;
}


////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	//!< Bicubic補間によるアップスケール
	
	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return;
	}
	
	float2 scale = (float2)GetInputDimension() / (float2)size;

	float2 position = (float2(index)) * scale;

	uint2 b  = (uint2)floor(position);
	float2 f = frac(position);

	float2 w[4];

	for (uint i = 0; i < 4; ++i) {
		float dx = (float)i - 1 - f.x;
		float dy = (float)i - 1 - f.y;

		w[i].x = CubicWeight(dx);
		w[i].y = CubicWeight(dy);

	}

	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float weight = 0.0f;

	for (uint i = 0; i < 4; ++i) {
		for (uint j = 0; j < 4; ++j) {
			uint2 pos = clamp(b + uint2(i - 1, j - 1), uint2(0, 0), GetInputDimension() - 1);

			float wgh = w[i].x * w[j].y;
			float4 c  = gInput[pos];

			color  += c * wgh;
			weight += wgh;
		}
	}

	color /= weight;

	float2 uv = (float2)index / (float2)size;

	gOutput[index] = gInput.SampleLevel(gSampler, uv, 0);
	
}
