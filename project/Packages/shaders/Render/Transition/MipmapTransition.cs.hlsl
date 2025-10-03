//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gInput  : register(u0); //!< 前level
RWTexture2D<float4> gOutput : register(u1); //!< 次level

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	//!< Outputをdimensionとして扱う

	uint2 index = dispatchThreadId.xy;

	if (CheckOverTexture(index)) {
		return;
	}

	uint2 sourceIndex = index * 2;

	float4 c0 = gInput[sourceIndex + uint2(0, 0)]; //!< 左上
	float4 c1 = gInput[sourceIndex + uint2(1, 0)]; //!< 右上
	float4 c2 = gInput[sourceIndex + uint2(0, 1)]; //!< 左下
	float4 c3 = gInput[sourceIndex + uint2(1, 1)]; //!< 右下

	gOutput[index] = (c0 + c1 + c2 + c3) * 0.25f;
	
}
