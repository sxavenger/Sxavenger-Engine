//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Common.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float> gEdgeWorking : register(t0);
Texture2D<float> gAOWorking : register(t1);
SamplerState gPointSampler : register(s0);

RWTexture2D<float> gAOWorkingOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)] //!< Dispatch数は(width / 2, height)で呼び出す.
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	uint2 pixcoord = dispatchThreadId.xy * uint2(2, 1); //!< 2x1ブロックを処理するため、ピクセル座標をスケーリング.
	
	if (CheckOverDimension(pixcoord)) {
		return; //!< pixcoordが範囲外.
	}
	
	GTAOConstants constants = SxGTAO::GTAOSettings::GetConstants(SxGTAO::sGTAOSettings, dimension, gCamera.proj);

	SxGTAO::XeGTAO_Denoise(pixcoord, constants, gEdgeWorking, gAOWorking, gPointSampler, gAOWorkingOutput, false);
	
}
