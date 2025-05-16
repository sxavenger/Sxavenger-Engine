//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput : register(t0); //!< input texture

RWTexture2D<float4> gOutput : register(u1); //!< output texture

struct Parameter {
	uint2 tile;
	// 条件: tileはx軸方向に分割されていること
	// todo: xy分割両方に対応する
};
ConstantBuffer<Parameter> gParameter : register(b0); //!< parameter

Texture2D<float4> gLUTTexture : register(t1); //!< LUT texture
SamplerState gLUTSampler      : register(s1);

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
	input = saturate(input); //!< [0, 1]に収める

	// 読み込む区間の決定
	float tileIndex = input.b * (gParameter.tile.x - 1);

	//const float kEveryTileUV = 1.0f / gParameter.tile.x;

	float sliceIndex0   = floor(tileIndex);
	float sliceIndex1   = min(sliceIndex0 + 1, gParameter.tile.x - 1);
	float interpolation = frac(tileIndex); //!< tileの中間割合値

	float2 uv0 = float2(
		(input.r / gParameter.tile.x) + (sliceIndex0 / gParameter.tile.x),
		input.g
	);

	float2 uv1 = float2(
		(input.r / gParameter.tile.x) + (sliceIndex1 / gParameter.tile.x),
		input.g
	);

	float3 color0 = gLUTTexture.SampleLevel(gLUTSampler, uv0, 0).rgb;
	float3 color1 = gLUTTexture.SampleLevel(gLUTSampler, uv1, 0).rgb;
	float3 color  = lerp(color0, color1, interpolation); //!< 線形補間

	gOutput[index] = float4(color, input.a);
	
}
