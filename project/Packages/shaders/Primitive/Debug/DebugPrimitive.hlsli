#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Camera.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
ConstantBuffer<Camera> gCamera : register(b0);
static const float4x4 viewProj = mul(gCamera.view, gCamera.proj);

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSInput {
	float4 position : POSITION0;
	float4 color    : COLOR0;
};

struct VSOutput {
	float4 position : SV_Position;
	float4 color    : COLOR0;
};

struct PSOutput {
	float4 color : SV_Target0;
};
