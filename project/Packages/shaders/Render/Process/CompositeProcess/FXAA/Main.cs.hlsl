//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Process.hlsli"

//* externals
#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_QUALITY__PRESET 39
#include "../../../../Externals/FXAA/Fxaa_3.11.hlsli"

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
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;
	
	if (CheckOverDimension(pixel)) {
		return; //!< texture dimension check
	}

	float2 texcoord = (float2(pixel) + 0.5f) / dimension;

	FxaaTex fxaaTex;
	fxaaTex.tex      = gInput;
	fxaaTex.smpl     = gSampler;
	fxaaTex.UVMinMax = float4(0.0f, 0.0f, 1.0f, 1.0f);

	float2 rcp = float2(1.0f / dimension.x, 1.0f / dimension.y);

	gOutput[pixel] = FxaaPixelShader(
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
