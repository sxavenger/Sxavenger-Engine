#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Transform.hlsli"
#include "../../Camera.hlsli"
#include "DeferredBuffer.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Input / Output structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSInput {
	float4 position : POSITION0;
};

struct PSInput {
	float4 position : SV_Position;
	uint instanceId : INSTANCE;
};

struct PSOutput {
	float4 color : SV_Target0;
	//* rgb: color, a: intensity
};

//=========================================================================================
// buffers
//=========================================================================================

//* t10 and t11 is defined in DeferredBuffer.hlsli
StructuredBuffer<TransformationMatrix> gTransform : register(t12);

ConstantBuffer<Camera> gCamera : register(b10);
static const float4x4 kViewProj = mul(gCamera.view, gCamera.proj);

struct Infomation {
	float2 size;
};
ConstantBuffer<Infomation> gInfo : register(b11);
