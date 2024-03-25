#include "Object3d.hlsli"

struct TransformationMatrix {
	float4x4 wvp;
	float4x4 world;
	float4x4 worldInverceTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

////////////////////////////////////////////////////////////////////////////////////////////
// メイン
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	output.position = mul(input.position, gTransformationMatrix.wvp);
	output.worldPos = mul(input.position, gTransformationMatrix.world);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3)gTransformationMatrix.worldInverceTranspose));
	
	return output;
}