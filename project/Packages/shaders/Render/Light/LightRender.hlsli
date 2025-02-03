#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Transform.hlsli"
#include "../../Camera.hlsli"
#include "../../Light.hlsli"
#include "DeferredBuffers.hlsli"

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

ConstantBuffer<Camera> gCamera : register(b10);
static const float4x4 kViewProj = mul(gCamera.view, gCamera.proj);

//* t10 ~ t13 is defined in DeferredBuffer.hlsli
StructuredBuffer<TransformationMatrix> gTransform : register(t14); //!< Light transform buffer

RaytracingAccelerationStructure gScene : register(t15);
