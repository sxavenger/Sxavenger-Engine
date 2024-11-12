#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Camera.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
ConstantBuffer<Camera> gCamera : register(b10);
static const float4x4 kViewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSInput {
	float4 positionLeft  : POSITION0;
	float4 positionRight : POSITION1;
	float4 color         : COLOR0;
};

struct VSOutput {
	float4 positionLeft  : POSITION0;
	float4 positionRight : POSITION1;
	float4 color         : COLOR0;
};

struct GSOutput {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR0;
};

struct PSOutput {
	float4 color : SV_Target0;
};