//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Primitive.hlsli"
#include "../Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================
ConstantBuffer<Camera> gCamera : register(b0);
static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	output.position = mul(input.position, viewProj);
	output.texcoord = input.texcoord;
	output.color    = input.color;
	
	return output;
	
}