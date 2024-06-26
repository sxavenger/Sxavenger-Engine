//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Object3d.hlsli"
#include "../camera.hlsli"

//=========================================================================================
// Input
//=========================================================================================
struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

//=========================================================================================
// Buffer
//=========================================================================================
ConstantBuffer<Camera> gCamera : register(b0);

struct TransformationMatrix {
	float4x4 world;
};
ConstantBuffer<TransformationMatrix> gTransform : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	output.position = mul(input.position, mul(gTransform.world, gCamera.viewProj));
	output.texcoord = input.texcoord;
	output.normal   = mul(input.normal, (float3x3)gTransform.world);
	output.worldPos = mul(input.position, gTransform.world).xyz;
	
	return output;
	
}
