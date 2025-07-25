#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSInput {
	float3 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float4 color    : COLOR0;
};

struct PSInput {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
	float4 color    : COLOR0;
};

struct PSOutput {
	float4 color : SV_Target0;
};

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gTexture : register(t10);
SamplerState gSampler      : register(s10);

cbuffer Dimension : register(b10) {
	uint2 size;
}

struct Transformation2d {

	//=========================================================================================
	// variables
	//=========================================================================================
	
	float4x4 mat;

	//=========================================================================================
	// methods
	//=========================================================================================

	float2 Transform(float2 v) {
		return mul(float4(v, 0.0f, 1.0f), mat).xy;
	}
	
};
ConstantBuffer<Transformation2d> gTransformation2d : register(b11);
ConstantBuffer<Transformation2d> gTransformationUV : register(b12);

