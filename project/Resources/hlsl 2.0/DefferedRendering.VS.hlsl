//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DefferedRendering.hlsli"

//=========================================================================================
// Input
//=========================================================================================
struct DefferedVSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
DefferedVSOutput main(DefferedVSInput input) {
	
	DefferedVSOutput output;
	
	output.position = input.position;
	output.texcoord = input.texcoord;
	
	return output;
}