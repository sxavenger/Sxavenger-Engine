#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../VertexStructure.hlsli"
#include "../../Camera3d.hlsli"
#include "../../Transform.hlsli"
#include "../../Material.hlsli"
#include "../SystematicFrame.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output strucutre
////////////////////////////////////////////////////////////////////////////////////////////

struct PSInput {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;
	float3 worldPos : POSITION0;
};

struct AdaptiveOutput {
	float4 color : SV_Target0;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Common methods
////////////////////////////////////////////////////////////////////////////////////////////

void CheckDiscard(float4 color) {
	if (color.a <= 0.0f) {
		discard;
	}
}

//=========================================================================================
// Visibility All Buffer
//=========================================================================================

ConstantBuffer<Camera3d> gCamera : register(b10);
static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);
