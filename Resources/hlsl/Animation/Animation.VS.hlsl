//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Animation.hlsli"
#include "../camera.hlsli"

//=========================================================================================
// Input
//=========================================================================================
struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
	float4 weight   : WEIGHT0;
	int4 index      : INDEX0;
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

struct Well {
	float4x4 skeletonSpaceMatrix;
	float4x4 skeletonSpaceInverseTransposeMatrix;
};
StructuredBuffer<Well> gMatrixPalette : register(t0);

//=========================================================================================
// methods
//=========================================================================================
struct Skinned {
	float4 position;
	float3 normal;
};

Skinned Skinning(VSInput input) {
	Skinned result;
	
	// positionの変換
	result.position  = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
	result.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
	result.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
	result.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
	result.position.w = 1.0f;
	
	// normalの変換
	result.normal  = mul(input.normal, (float3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
	result.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
	result.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
	result.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
	result.normal  = normalize(result.normal);
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	Skinned skinned = Skinning(input);
	
	output.position = mul(skinned.position, mul(gTransform.world, viewProj));
	output.texcoord = input.texcoord;
	output.normal   = mul(skinned.normal, (float3x3)gTransform.world);
	output.worldPos = mul(skinned.position, gTransform.world).xyz;
	
	return output;
	
}
