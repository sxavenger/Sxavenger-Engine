//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Common.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float> gAOWorking : register(t0);
RWTexture2D<float4> gMaterialARM : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	uint2 pixcoord = dispatchThreadId.xy; //!< ピクセル座標.
	
	if (CheckOverDimension(pixcoord)) {
		return; //!< pixcoordが範囲外.
	}
	
	gMaterialARM[pixcoord].r = gAOWorking[pixcoord]; //!< AOをRチャンネルに書き込む.
	
}
