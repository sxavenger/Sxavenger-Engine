#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* particle
#include "Particle.hlsli"

//* gometry
#include "../Geometry/GeometryRenderTarget.hlsli"
#include "../Geometry/GeometryVertex.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// intermediate structure
////////////////////////////////////////////////////////////////////////////////////////////

struct ParticlePSInput {
	float4 position  : SV_Position;
	float2 texcoord  : TEXCOORD0;
	float3 normal    : NORMAL0;

	uint instanceId : INSTANCE0;
};

//=========================================================================================
// common buffer
//=========================================================================================

ConstantBuffer<CameraComponent> gCamera : register(b10);
static const float4x4 kViewProj = gCamera.GetViewProj();

StructuredBuffer<TransformComponent> gTransforms : register(t10);
StructuredBuffer<Particle>           gParticles  : register(t11);
