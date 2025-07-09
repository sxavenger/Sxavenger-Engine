#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "GeometryVertex.hlsli"
#include "GeometryRenderTarget.hlsli"

//* content
#include "../../Content/Material.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"

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
	uint instanceId  : INSTANCEID;
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

ConstantBuffer<CameraComponent> gCamera : register(b10);
static const float4x4 kViewProj = gCamera.GetViewProj();

StructuredBuffer<TransformComponent> gTransforms : register(t10);
