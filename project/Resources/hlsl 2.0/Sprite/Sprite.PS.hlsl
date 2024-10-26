//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Sprite.hlsli"
#include "../Matrix.hlsli"

//=========================================================================================
// TextureBuffer
//=========================================================================================
Texture2D<float4> gTexture : register(t0);
SamplerState      gSampler : register(s0);

//=========================================================================================
// Constants
//=========================================================================================
struct SpriteMaterial {
	float4   color;
	float4x4 uvTransform;
	float2   pivot;
};
ConstantBuffer<SpriteMaterial> gMaterial : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSoutput main(VSOutput input) {
	
	PSoutput output;
	
	float4x4 uvToPivotMat = MakeTranslate(float3(-gMaterial.pivot, 0.0f));
	float4x4 pivotToUVMat = MakeTranslate(float3(gMaterial.pivot, 0.0f));
	
	float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), mul(mul(uvToPivotMat, gMaterial.uvTransform), pivotToUVMat));
	output.color = gTexture.Sample(gSampler, transformedUV.xy);
	
	return output;
	
}