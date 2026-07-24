//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Common.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float> gDepth    : register(t0);
SamplerState gPointSampler : register(s0);

RWTexture2D<float> gDepthWorkingMip0 : register(u0);
RWTexture2D<float> gDepthWorkingMip1 : register(u1);
RWTexture2D<float> gDepthWorkingMip2 : register(u2);
RWTexture2D<float> gDepthWorkingMip3 : register(u3);
RWTexture2D<float> gDepthWorkingMip4 : register(u4);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(8, 8, 1)] // <- hard coded to 8x8; each thread computes 2x2 blocks so processing 16x16 block: Dispatch needs to be called with (width + 16-1) / 16, (height + 16-1) / 16
void main(uint3 dispatchThreadId : SV_DispatchThreadID, uint2 groupThreadId : SV_GroupThreadID) {

	uint2 pixcoord = dispatchThreadId.xy;

	if (CheckOverDimension(pixcoord)) {
		return; //!< pixcoordが範囲外.
	}

	GTAOConstants constants = SxGTAO::GTAOSettings::GetConstants(SxGTAO::sGTAOSettings, dimension, gCamera.proj);
	
	SxGTAO::XeGTAO_PrefilterDepths16x16(pixcoord.xy, groupThreadId.xy, constants, gDepth, gPointSampler, gDepthWorkingMip0, gDepthWorkingMip1, gDepthWorkingMip2, gDepthWorkingMip3, gDepthWorkingMip4);

}
