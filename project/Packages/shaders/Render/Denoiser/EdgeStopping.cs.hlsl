//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Denoiser.hlsli"
#include "../DeferredBuffers.hlsli"
#include "../../Library/Math.hlsli"

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
	float sigma_z;
	float sigma_s;
};

//static const float sigma_n = 1.0f;
//static const float sigma_z = 0.1f;
//static const float sigma_s = 0.1f;

//* input texture
Texture2D<float4> gIndirect : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREAD_X, _NUM_THREAD_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;

	if (CheckOutOfRange(index)) {
		return;
	}

	Surface surface;
	surface.GetSurface(index);

	float weight = 0.0f;
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	static const int A = 3;

	// test
	for (int dx = -A; dx <= A; ++dx) {
		for (int dy = -A; dy <= A; ++dy) {

			int2 sample_pos = index + int2(dx, dy);

			if (CheckOutOfRange(sample_pos)) {
				continue;
			}

			Surface sample_surface;
			sample_surface.GetSurface(sample_pos);

			float exp_w = 0.0f;
			exp_w += -abs(surface.depth - sample_surface.depth) / (sigma_z + kEpsilon);                                                        //!< 深度
			exp_w += -abs(dot(surface.position - sample_surface.position, surface.position - sample_surface.position)) / (sigma_s + kEpsilon); //!< 空間位置

			float w = exp(exp_w);
			w *= pow(max(0.0f, dot(surface.normal, sample_surface.normal)), sigma_n); //!< 法線

			color  += gIndirect[sample_pos] * w;
			weight += w;
		}
	}

	color /= weight;

	gOutput[index] = color;
}
