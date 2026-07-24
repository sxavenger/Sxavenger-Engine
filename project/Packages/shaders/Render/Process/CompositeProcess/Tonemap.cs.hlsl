//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Process.hlsli"
#include "../../../Library/ACES.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ColotSpace namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ColorSpace {
	static const uint kRec_709          = 0,
	                  kRec_2020_1000nit = 1,
	                  kRec_2020_2000nit = 2;
}

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gTexture : register(u0); //!< input and output texture

cbuffer ColorSpaceBuffer : register(b0) {
	uint space;
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverDimension(index)) {
		return; //!< texture size over
	}

	float3 input = gTexture[index].rgb;

	float3 ap0  = mul(ACES::AP1_2_AP0_Matrix, input);
	float3 oces = ACES::RRT(ap0);

	float3 output = float3(0.0f, 0.0f, 0.0f);
	
	switch (space) {
		case ColorSpace::kRec_709:
			output = ACES::ODT_sRGB_D65(oces);
			break;
		
		case ColorSpace::kRec_2020_1000nit:
			output = ACES::ODT_1000nits(oces);
			break;
		
		case ColorSpace::kRec_2020_2000nit:
			output = ACES::ODT_2000nits(oces);
			break;
		
		default:
			break;
	}

	gTexture[index].rgb = output;
}
