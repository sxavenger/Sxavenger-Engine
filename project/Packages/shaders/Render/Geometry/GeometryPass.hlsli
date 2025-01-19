#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GeometryVertex.hlsli"
#include "GeometryRenderTarget.hlsli"
#include "../../Camera.hlsli"
#include "../../Transform.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// intermediate structure
////////////////////////////////////////////////////////////////////////////////////////////

struct GeometryPSInput {
	float4 position  : SV_Position;
	float2 texcoord  : TEXCOORD0;
	float3 normal    : NORMAL0;
	float3 worldPos  : POSITION0;
	float3 tangent   : TANGENT0;
	float3 bitangent : BITANGENT0;
};

////////////////////////////////////////////////////////////////////////////////////////////
// common method
////////////////////////////////////////////////////////////////////////////////////////////

float3x3 GetTangentSpaceMatrix(float3 normal, float3 tangent, float3 bitangent) {
	return float3x3(tangent, bitangent, normal);
}

//=========================================================================================
// common buffer
//=========================================================================================

ConstantBuffer<Camera> gCamera : register(b10);
static const float4x4 kViewProj = gCamera.GetViewProj();

struct TextureComponent {
	float4x4 mat;
	float4 color;

	//* methods *//
	
	float2 Transform2d(float2 texcoord) {
		return mul(float4(texcoord, 0.0f, 1.0f), mat).xy;
	}
	
};
ConstantBuffer<TextureComponent> gTextureComponent : register(b11);

