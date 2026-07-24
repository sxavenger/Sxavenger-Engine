//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gInput  : register(u0); //!< input texture
RWTexture2D<float4> gOutput : register(u1); //!< output texture

struct Parameter {
	float3 color;
	float2 center;
	float exponent;
};
ConstantBuffer<Parameter> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverDimension(index)) {
		return; //!< texture size over
	}

	float4 input = gInput[index];

	float2 uv      = (float2(index) + 0.5f) / float2(dimension) * 2.0f - 1.0f; // [-1.0 ~ 1.0]
	float distance = length(uv - gParameter.center);

	float t = pow(saturate(distance), gParameter.exponent);

	
	//gOutput[index] = color;
}
