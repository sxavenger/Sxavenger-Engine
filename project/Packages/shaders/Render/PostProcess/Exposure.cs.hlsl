/*
# reference
 - https://dev.epicgames.com/documentation/en-us/unreal-engine/auto-exposure-in-unreal-engine
 - https://note.com/artemisia_franc/n/nbf650de25f0d
*/

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
	float f;            //!< 絞り(Apreture)
	float shutterSpeed; //!< シャッタースピード(Shutter Speed) [1/sec]: 1.0f / t
	float iso;          //!< ISO感度(ISO)
	float compensation; //!< 露出補正(Exposure Compensation)
};
ConstantBuffer<Parameter> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float4 input = gInput[index];
	
	float ev100    = log2((gParameter.f * gParameter.f / gParameter.shutterSpeed) * (100.0f / gParameter.iso));
	float exposure = 1.0f / exp2(ev100) + gParameter.compensation;
	float4 output  = input * exposure;

	gOutput[index] = output;
}
