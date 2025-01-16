#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float4 color    : COLOR0;
};

struct VSOutput {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
	float4 color    : COLOR0;
};

struct PSOutput {
	float4 color : SV_Target0;
};

//=========================================================================================
// Visibility All Buffers
//=========================================================================================

Texture2D<float4> gTexture : register(t10);
SamplerState gSampler      : register(s10);

struct Transform2d {
	float4x4 transform2d;
};
ConstantBuffer<Transform2d> gTransform : register(b10);

struct Projection {
	float4x4 proj;
};
ConstantBuffer<Projection> gProjection : register(b11);
