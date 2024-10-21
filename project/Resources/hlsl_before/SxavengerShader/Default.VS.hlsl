//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Common
#include "SxavengerCommon.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
ConstantBuffer<Camera> gCamera : register(b0);
static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

ConstantBuffer<TransformationMatrix> gTransform : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	output.position = mul(input.position, mul(gTransform.world, viewProj));
	output.texcoord = input.texcoord;
	output.normal   = normalize(mul(input.normal, (float3x3)gTransform.worldInverseTranspose));
	output.worldPos = mul(input.position, gTransform.world).xyz;
	
	return output;
	
}