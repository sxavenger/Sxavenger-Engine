#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

// shaders
#define _RAYGENERATION [shader("raygeneration")]
#define _ANYHIT        [shader("anyhit")]
#define _CLOSESTHIT    [shader("closesthit")]
#define _MISS          [shader("miss")]

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/LightComponentCommon.hlsli"
#include "../../Component/DirectionalLightComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"

//=========================================================================================
// global buffers
//=========================================================================================

//* raytracing output textures.
RWTexture2D<float4> gOutput : register(u0, space1);
RWTexture2D<float>  gDepth  : register(u1, space1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0, space1);
static const float4x4 kViewProj = gCamera.GetViewProj();

//* scene
RaytracingAccelerationStructure gScene : register(t0, space1);

//* light
struct LightCount {
	uint count;
};

//# Directional Light
ConstantBuffer<LightCount> gDirectionalLightCount                : register(b1, space1);
StructuredBuffer<TransformComponent> gDirectionalLightTransforms : register(t1, space1);
StructuredBuffer<DirectionalLightComponent> gDirectionalLights   : register(t2, space1);
StructuredBuffer<InlineShadow> gDirectionalLightShadows          : register(t3, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// config variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kTMin = 0.001f;   // minimum t value for ray intersection
static const float kTMax = 16384.0f; // maximum t value for ray intersection

static const uint kFlag    = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
static const uint kRayMask = 0xFF;

////////////////////////////////////////////////////////////////////////////////////////////
// Payload structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float4 color;
	float depth;

	//=========================================================================================
	// public methods
	//=========================================================================================

	
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// Attribtue structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Attribute {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float2 barys;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float3 GetBarycentrics() {
		return float3(
			1.0f - barys.x - barys.y,
			barys.x,
			barys.y
		);
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

void TraceRay(inout Payload payload, RayDesc desc, uint flag = kFlag) {
	TraceRay(gScene, flag, kRayMask, 0, 1, 0, desc, payload);
}
