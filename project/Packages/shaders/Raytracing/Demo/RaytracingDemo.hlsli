#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gOutput            : register(u10);
RaytracingAccelerationStructure gScene : register(t10);

ConstantBuffer<Camera> gCamera : register(b10);

////////////////////////////////////////////////////////////////////////////////////////////
// common static variables
////////////////////////////////////////////////////////////////////////////////////////////

static const RAY_FLAG kFlag = RAY_FLAG_NONE;
static const uint kRayMask  = 0xFF;

static const float kDefaultTMin = 0.001f;
static const float kDefaultTMax = 100000.0f;

////////////////////////////////////////////////////////////////////////////////////////////
// common strcutre
////////////////////////////////////////////////////////////////////////////////////////////

struct Payload {
	float3 color;
};

struct Attribute {
	float2 barys;
};

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

void TraceRay(RayDesc rayDesc, inout Payload payload) {
	TraceRay(gScene, kFlag, kRayMask, 0, 1, 0, rayDesc, payload);
}



