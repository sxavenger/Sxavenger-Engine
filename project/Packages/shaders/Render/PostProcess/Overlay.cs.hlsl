//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"
#include "Bloom.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gInputDst    : register(u1);
RWTexture2D<float4> gInputSrc    : register(u0);

RWTexture2D<float4> gOutput : register(u2);

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessOutput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ProcessOutput {
	//* member *//
	
	float4 color;
	
	//* method *//
	
	void SetOutput(uint2 index) {
		gOutput[index] = color;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispathThreadId : SV_DispatchThreadID) {
	
	uint2 index = dispathThreadId.xy;

	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}
	
	ProcessOutput output = (ProcessOutput)0;

	float4 dst = gInputDst[index]; // base pixel
	float4 src = gInputSrc[index]; // overlay pixel
	
	output.color.rgb = dst.rgb + saturate(src.rgb);
	output.color.a = saturate(dst.a + src.a);
	output.SetOutput(index);
	
}
