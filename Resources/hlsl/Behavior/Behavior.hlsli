#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Camera.hlsli"
#include "../Transform.hlsli"
#include "../SystematicFrame.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output strucutre
////////////////////////////////////////////////////////////////////////////////////////////

struct VertexInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
};

struct PSInput {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
	float3 worldPos : POSITION0;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Common methods
////////////////////////////////////////////////////////////////////////////////////////////

void CheckDiscard(float4 color) {
	if (color.a == 0.0f) {
		discard;
	}
}

//=========================================================================================
// Visibility All Buffer
//=========================================================================================

ConstantBuffer<Camera> gCamera : register(b10);
static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);
