//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyVisibilityCommon.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gDirect : register(u0);

Texture2D<float4> gHistory : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		gDirect[pixel] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; //!< 範囲外のスレッドは処理しない
	}

	float3 history = gHistory[pixel].rgb;

	gDirect[pixel].rgb = history; //!< 直接光は強調して表示する
	gDirect[pixel].a   = 1.0f;
	
}
