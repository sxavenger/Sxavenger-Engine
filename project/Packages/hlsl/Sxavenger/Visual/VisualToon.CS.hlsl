//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "VisualProcess.hlsli"
#include "../../ColorUtils.hlsli"

//=========================================================================================
// Input
//=========================================================================================

Texture2D<float4> gInput : register(t0);

//=========================================================================================
// Output
//=========================================================================================

RWTexture2D<float4> gOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUMTHREADS_X, _NUMTHREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 currentId = dispatchThreadId.xy;
	uint2 size      = gConfig.size;

	// 画像サイズ以上の場合, 書き込みをしない
	if (any(currentId >= size)) {
		return;
	}

	float4 input  = gInput[currentId];
	float4 output = (float4)0;

	output.a = input.a;
	output.rgb = ToonShading(input.rgb, 12);

	gOutput[currentId] = output;
}