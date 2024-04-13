//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Floor.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// VSInput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

////////////////////////////////////////////////////////////////////////////////////////////
// StructuredBuffers
////////////////////////////////////////////////////////////////////////////////////////////
struct FloorForGPU {
	float4x4 wvp;
	float4x4 world;
	float4x4 worldInverseTranspose;
};
ConstantBuffer<FloorForGPU> gFloor : register(b0);

// todo: 非均一スケールの対応

////////////////////////////////////////////////////////////////////////////////////////////
// メイン
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {
	
	VSOutput output;
	
	output.position = mul(input.position, gFloor.wvp);
	output.worldPos = mul(input.position, gFloor.world);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3)gFloor.world));
	
	return output;
	
}