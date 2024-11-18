#pragma once

struct VSInput {
	float4 position : POSITION0;
};

struct VSOutput {
	float4 position : SV_Position;
};

struct PSOutput {
	float4 color : SV_Target0;
};