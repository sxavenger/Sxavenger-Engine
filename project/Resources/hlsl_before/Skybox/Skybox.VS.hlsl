//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Skybox.hlsli"
#include "../Camera.hlsli"

//=========================================================================================
// Input
//=========================================================================================
struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

//=========================================================================================
// Buffer
//=========================================================================================
ConstantBuffer<Camera> gCamera : register(b0);
static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

struct TransformationMatrix {
	float4x4 world;
};
ConstantBuffer<TransformationMatrix> gTransform : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	output.position = mul(input.position, mul(gTransform.world, viewProj)).xyww; //!< zをw(1.0f)として絶対に後ろにあるようにする
	output.texcoord = input.position.xyz;

	return output;
}