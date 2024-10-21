#pragma once

//=========================================================================================
// Input
//=========================================================================================

struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float4 color    : COLOR0;
};

//=========================================================================================
// Output
//=========================================================================================

struct VSOutput {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
	float4 color    : COLOR0;
};

struct PSOutput {
	float4 color : SV_Target0;
};