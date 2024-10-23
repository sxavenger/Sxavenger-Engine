//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DebugObject.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
struct WorldTransformationMatrix {
	float4x4 worldMatrix;
	float4x4 worldInverseTranspose;
};
ConstantBuffer<WorldTransformationMatrix> gTransform : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	output.position = mul(input.position, mul(gTransform.worldMatrix, viewProj));
	output.worldPos = mul(input.position, gTransform.worldMatrix).xyz;
	output.texcoord = input.texcoord;
	output.normal   = mul(input.normal, (float3x3)gTransform.worldMatrix);
	// fixed: gTransform use worldInverseTranspose
	
	return output;
}