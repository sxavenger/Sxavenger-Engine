#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Camera.hlsli"

//=========================================================================================
// Config structure
//=========================================================================================

struct Payload {
	float4 color;
};

struct Attribute {
	float2 barys;
};

struct Vertex {
	float4 position;
	float2 texcoord;
	float3 normal;
};

//=========================================================================================
// Config variables
//=========================================================================================

static const float kTmin = 0.0001f;
static const float kTmax = 8192.0f;

//=========================================================================================
// TLAS Buffer
//=========================================================================================

RaytracingAccelerationStructure gScene : register(t10);

//=========================================================================================
// Global buffer
//=========================================================================================

ConstantBuffer<Camera> gCamera : register(b10);

//* output
RWTexture2D<float4> gOutput : register(u10);

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

void TraceRay(inout Payload payload, RayDesc desc) {

	RAY_FLAG flags = RAY_FLAG_NONE;
	uint rayMask   = 0xFF;

	TraceRay(
		gScene,
		flags,
		rayMask,
		0,
		1,
		0,
		desc,
		payload
	);
}

inline float3 CalcBarycentrics(float2 barys) {
	return float3(
		1.0f - barys.x - barys.y,
		barys.x,
		barys.y
	);
}