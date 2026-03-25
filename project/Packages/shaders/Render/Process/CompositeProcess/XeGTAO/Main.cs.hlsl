//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Common.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float> gDepthWorking : register(t0);
SamplerState gPointSampler     : register(s0);

Texture2D<float4> gNormal : register(t1);

RWTexture2D<float> gAOWorking   : register(u0);
RWTexture2D<float> gEdgeWorking : register(u1);

cbuffer RandomConstant : register(b0) {
	uint random;
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixcoord = dispatchThreadId.xy;

	if (CheckOverDimension(pixcoord)) {
		return; //!< pixcoordが範囲外.
	}

	GTAOConstants constants     = SxGTAO::GTAOSettings::GetConstants(SxGTAO::sGTAOSettings, dimension, gCamera.proj);
	SxGTAO::GTAOQuality quality = SxGTAO::GTAOQuality::GetHigh();

	float3 normal     = gNormal.Load(int3(pixcoord, 0)).xyz * 2.0f - 1.0f; //!< [-1, 1]の範囲に変換.
	float3 viewNormal = mul(normal, (float3x3)gCamera.view); //!< ビュー空間の法線.

	float2 noise = SxGTAO::SpatioTemporalNoise(pixcoord, random);

	SxGTAO::XeGTAO_MainPass(pixcoord, quality, constants, viewNormal, noise, gDepthWorking, gPointSampler, gAOWorking, gEdgeWorking);
}
