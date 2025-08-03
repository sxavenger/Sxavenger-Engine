#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* particle
#include "../ParticleLib.hlsli"

//* gometry
#include "../../Geometry/GeometryRenderTarget.hlsli"
#include "../../Geometry/GeometryVertex.hlsli"

//* component
#include "../../../Component/CameraComponent.hlsli"
#include "../../../Component/GPUParticleComponent.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// intermediate structure
////////////////////////////////////////////////////////////////////////////////////////////

struct ParticlePSInput {
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL0;

	float2 projection : PROJECTION0;

	uint instanceId : INSTANCE0;
};

//=========================================================================================
// common buffer
//=========================================================================================

ConstantBuffer<CameraComponent> gCamera : register(b10);
static const float4x4 kViewProj = gCamera.GetViewProj();

ConstantBuffer<ParticleConfig> gConfig : register(b11);

StructuredBuffer<GPUParticleComponent> gParticles : register(t10);
