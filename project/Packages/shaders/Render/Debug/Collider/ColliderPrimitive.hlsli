#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Component/CameraComponent.hlsli"

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

ConstantBuffer<CameraComponent> gCamera : register(b0);
static const float4x4 kViewProj = gCamera.GetViewProj();

struct Parameter {
	float4x4 mat;
	float4 color;
};
StructuredBuffer<Parameter> gParameter : register(t0);
