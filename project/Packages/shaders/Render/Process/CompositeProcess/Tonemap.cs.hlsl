//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Process.hlsli"
#include "../../../Library/ACES.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gTexture : register(u0); //!< input and output texture

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float3 input = gTexture[index].rgb;

	float3 ap0          = mul(ACES::AP1_2_AP0_Matrix, input);
	float3 oces         = ACES::RRT(ap0);
	gTexture[index].rgb = ACES::ODT_sRGB_D65(oces);
	// todo: HDR出力に対応
	

}
