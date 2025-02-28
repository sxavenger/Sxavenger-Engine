#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Camera.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSInput {
	float4 position : POSITION;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float4 color    : COLOR;
};

struct PSOutput {
	float4 color : SV_TARGET;
};

////////////////////////////////////////////////////////////////////////////////////////////
// buffer
////////////////////////////////////////////////////////////////////////////////////////////

ConstantBuffer<Camera> gCamera : register(b0);
static const float4x4 kViewProj = mul(gCamera.view, gCamera.proj);

struct Parameter {
	float4x4 mat;
	float4 color;
};
ConstantBuffer<Parameter> gParameter : register(b1);
