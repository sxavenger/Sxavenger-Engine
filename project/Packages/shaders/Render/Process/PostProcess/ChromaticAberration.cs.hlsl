//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* textures
Texture2D<float4>   gInput  : register(t0);
RWTexture2D<float4> gOutput : register(u0);
SamplerState gSampler : register(s0);

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

	float2 uv        = float2(index) / size * 2.0f - 1.0f; // [-1.0 ~ 1.0]
	float2 direction = normalize(uv) * gParameter.GetIntensity() * blendWeight;

	float distance = saturate(length(uv));

	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	color.r = gInput.Sample(gSampler, (uv - direction * distance + 1.0f) * 0.5f).r;
	color.g = gInput.Sample(gSampler, (uv + 1.0f) * 0.5f).g;
	color.b = gInput.Sample(gSampler, (uv + direction * distance + 1.0f) * 0.5f).b;
	color.a = gInput[index].a;
	
	gOutput[index] = color;

}
