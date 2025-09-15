#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../DeferredBuffers.hlsli"

//* library
#include "../../Library/BRDF.hlsli"
#include "../../Library/Lighting.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSInput2d {
	float2 position : POSITION0;
};

struct VSInput3d {
	float4 position : POSITION0;
};

struct PSInput {
	float4 position : SV_Position;
	uint instanceId : INSTANCE;
};

struct PSOutput {
	float4 color : SV_Target0;
};


//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<CameraComponent> gCamera : register(b10);
static const float4x4 kViewProj = gCamera.GetViewProj();

StructuredBuffer<TransformComponent> gTransforms : register(t10); //!< Light transform buffer

RaytracingAccelerationStructure gScene : register(t11);

////////////////////////////////////////////////////////////////////////////////////////////
// static const variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kTMin = 0.001f;
static const float kTMax = 10000.0f;
