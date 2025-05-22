/*
# reference
 - https://dev.epicgames.com/documentation/en-us/unreal-engine/auto-exposure-in-unreal-engine
 - https://note.com/artemisia_franc/n/nbf650de25f0d
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Process.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput    : register(t0); //!< input texture
RWTexture2D<float4> gOutput : register(u0); //!< output texture

struct Parameter {
	float f;            //!< 絞り
	float shutterSpeed; //!< シャッタースピード(Shutter Speed) [1/sec]
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

	if (input.a <= 0.0f) {
		gOutput[index] = input;
		return;
	}

	float t = 1.0f / gParameter.shutterSpeed;
	
	float ev100 = log2((gParameter.f * gParameter.f / t) * (100.0f / gParameter.iso));
	float exposure = 1.0f / exp2(ev100 - gParameter.compensation);
	float3 output  = input.rgb * exposure;

	gOutput[index] = float4(output, input.a);
}
