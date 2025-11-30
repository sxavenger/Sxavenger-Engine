//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Process.hlsli"

#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_QUALITY__PRESET 39
#include "Fxaa_3.11.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput : register(t0);
SamplerState gSampler : register(s0);

RWTexture2D<float4> gOutput : register(u0);

cbuffer Parameter : register(b0) {
	float subpixel;
	float edgeThreshold;
	float edgeThresholdMin;
}

static const float kNone = 0.0f;

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float2 texcoord = (float2)index / (float2)size;

	FxaaTex fxaaTex;
	fxaaTex.tex      = gInput;
	fxaaTex.smpl     = gSampler;
	fxaaTex.UVMinMax = float4(0.0f, 0.0f, 1.0f, 1.0f);

	float2 rcp = float2(1.0f / size.x, 1.0f / size.y);

	gOutput[index] = FxaaPixelShader(
		texcoord,
		kNone,
		fxaaTex,
		fxaaTex,
		fxaaTex,
		rcp,
		kNone,
		kNone,
		kNone,
		subpixel,
		edgeThreshold,
		edgeThresholdMin,
		kNone,
		kNone,
		kNone,
		kNone
	);

}
