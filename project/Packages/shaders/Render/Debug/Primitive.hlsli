#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Component/CameraComponent.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

ConstantBuffer<CameraComponent> gCamera : register(b0);
static const float4x4 kViewProj = gCamera.GetViewProj();

cbuffer Infomation : register(b1) {
	uint2 size;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSInput {
	float4 position : POSITION0;
	float4 color    : COLOR0;
	float thickness : THICKNESS0;
};

struct GSInput {
	float4 position : POSITION0;
	float4 color    : COLOR0;
	float thickness : THICKNESS0;
};

struct PSInput {
	float4 position : SV_Position;
	float4 color    : COLOR0;
};

struct PSOutput {
	float4 color : SV_Target0;
};
