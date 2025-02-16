#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Component/TextureComponent.hlsli"
#include "../Component/Transform2dComponent.hlsli"

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

struct Parameter {
	uint2 size;
};
ConstantBuffer<Parameter> gParameter : register(b10);

ConstantBuffer<Transform2dComponent> gTransform2d  : register(b11);
ConstantBuffer<TextureComponent> gTextureComponent : register(b12);

