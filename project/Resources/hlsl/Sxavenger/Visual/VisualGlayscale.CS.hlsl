//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "VisualProcess.hlsli"

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

	float value = dot(input.rgb, float3(0.2125f, 0.7154f, 0.0721f));
	// TODO: ConstantBufferで持ってくるように改良
	
	output.rgb  = value * float3(1.0f, 74.0f / 107.0f, 43.0f / 107.0f);
	output.a    = input.a;

	gOutput[currentId] = output;
}