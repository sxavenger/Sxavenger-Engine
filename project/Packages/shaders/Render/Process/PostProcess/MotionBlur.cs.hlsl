//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"

#include "../../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* textures
Texture2D<float4>   gInput  : register(t0);
RWTexture2D<float4> gOutput : register(u0);
SamplerState gSampler : register(s0);

Texture2D<float4> gVelocity : register(t1);
ConstantBuffer<CameraComponent> gCamera : register(b1);

struct Parameter {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float intensity;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float GetIntensity() {
		return intensity * 0.01f;
	}
	
};
ConstantBuffer<Parameter> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispathThreadId : SV_DispatchThreadID) {

	uint2 index = dispathThreadId.xy;

	if (CheckOverTexture(index.xy)) {
		return;
	}

	float2 uv = float2(index) / size * 2.0f - 1.0f; // [-1.0 ~ 1.0]

	float3 velocity = -gVelocity.Load(uint3(index, 0)).xyz;
	
	float4 color = gInput.Load(uint3(index, 0));
	float weight = 1.0f;

	static const uint kSampleCount = 8;

	float2 v = velocity.xy / kSampleCount;
	
	for (uint i = 1; i < 8; ++i) {

		// ガウス関数
		float w = exp(-float(i * i) / (8.0f));
		
		float2 texcoord = uv + (v * i) /** (gParameter.GetIntensity() * blendWeight)*/;
		color += gInput.SampleLevel(gSampler, (texcoord + 1.0f) * 0.5f, 0) * w;
		weight += w;
	}

	color /= weight;

	gOutput[index] = color;

}
