#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Camera3d.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
ConstantBuffer<Camera3d> gCamera : register(b0);
static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

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