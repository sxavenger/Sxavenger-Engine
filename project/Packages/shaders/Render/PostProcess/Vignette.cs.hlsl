//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"
#include "../../Blend.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gInput  : register(u0); //!< input texture
RWTexture2D<float4> gOutput : register(u1); //!< output texture

struct Parameter {
	float4 color;
	float offset;
	float power;
};
ConstantBuffer<Parameter> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float4 input = gInput[index];

	float2 uv     = float2(index) / float2(gConfig.size - 1);
	float2 center = float2(0.5f, 0.5f);

	float l = length(uv - center) * gParameter.offset;
	float t = 1.0 - saturate(l * 2.0f); //!< [0.0 ~ 1.0]

	float v = pow(t, gParameter.power);

	float4 color = BlendNormalAlpha(float4(gParameter.color.rgb, 1.0f - v), input);
	//!< todo: blend modeおかしい

	gOutput[index] = color;
}
