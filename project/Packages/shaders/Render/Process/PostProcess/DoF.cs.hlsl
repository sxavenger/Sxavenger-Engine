/*
# reference
 - https://dev.epicgames.com/documentation/en-us/unreal-engine/cinematic-depth-of-field-in-unreal-engine
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Process.hlsli"
#include "../../DeferredBuffers.hlsli"
#include "../../../Library/Gaussian.hlsli"

//* component
#include "../../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput : register(t0); //!< input texture
SamplerState gSampler    : register(s0);

RWTexture2D<float4> gOutput : register(u0); //!< output texture

ConstantBuffer<CameraComponent> gCamera : register(b0);

struct Parameter {
	float focalDistance;

	uint isDebugView;
};
ConstantBuffer<Parameter> gParameter : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

/*
# reference
 - https://shikihuiku.github.io/post/projection_matrix/
*/
float FixLinearDepth(float depth) {
	return depth / (gCamera.far / gCamera.near - depth * (gCamera.far / gCamera.near - 1.0f));
}

float CalculateDelta(float depth) {
	float delta = FixLinearDepth(depth) * (gCamera.far - gCamera.near) - gParameter.focalDistance;

	const float nearTransition = 2.0f;
	const float farTransition  = 2.0f;

	if (delta > 0.0f) { // far
		return saturate(delta /*/ farTransition*/);
		
	} else { // near
		return -saturate(-delta /*/ nearTransition*/);
	}

	//* 戻り値は [-1.0, 1.0] の符号付き値である.
	//* [-1.0, 0.0] は near
	//* [0.0, 1.0] は far
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

	Surface surface;
	if (!surface.GetSurface(index)) {
		gOutput[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; //!< surface is not exist.
	}

	float delta = CalculateDelta(surface.depth);

	if (gParameter.isDebugView) {

		float near = saturate(-delta);
		float far  = saturate(delta);
		
		gOutput[index] = float4(0.0f, near, far, 1.0f);
		return;
	}

	// HACK: ちゃんとしたアルゴリズムに変更
	//* https://epicgames.ent.box.com/s/s86j70iamxvsuu6j35pilypficznec04
	
	static const int2 kSampleSize = int2(16, 16);
	//!< todo: 円形サンプリングに変更

	static const float kSigma = 4.0f;

	float4 color_sum = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float weight_sum = 0.0f;

	for (int x = -kSampleSize.x; x <= kSampleSize.x; ++x) {
		for (int y = -kSampleSize.y; y <= kSampleSize.y; ++y) {

			int2 sampleIndex  = index.xy + int2(x, y);
			float sampleDelta = CalculateDelta(gDepth.Load(uint3(sampleIndex, 0)));
			
			float2 uv = float2(sampleIndex) / float2(gConfig.size);

			float4 color = gInput.SampleLevel(gSampler, uv, 0);
			float weight = Gaussian2d(float2(x, y), kSigma);

			color_sum  += color * weight;
			weight_sum += weight;
		}
	}

	float4 color = color_sum / weight_sum;

	gOutput[index] = gInput.Load(uint3(index, 0)) * (1.0f - abs(delta)) + color * abs(delta);
	
}

