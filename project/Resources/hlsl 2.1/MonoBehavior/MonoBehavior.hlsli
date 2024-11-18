#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Camera.hlsli"
#include "../Transform.hlsli"

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

struct PSOutput {
	float4 color : SV_Target0;
};

struct SceneOutput { //!< SceneRenderTarget.h
	float4 albedo   : SV_Target0;
	float4 normal   : SV_Target1;
	float4 position : SV_Target2;
};

//=========================================================================================
// Visibility All Buffer
//=========================================================================================

ConstantBuffer<Camera> gCamera : register(b10);
static const float4x4 viewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

////////////////////////////////////////////////////////////////////////////////////////////
// Common methods
////////////////////////////////////////////////////////////////////////////////////////////

void CheckDiscard(float4 color) {
	if (color.a == 0.0f) {
		discard;
	}
}