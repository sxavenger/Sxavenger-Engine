#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../../../Library/RaytracingLib.hlsli"

//* component
#include "../../../Component/CameraComponent.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Config variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kTMin = 0.001f;
static const float kTMax = 10000.0f;

static const uint kFlag    = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
static const uint kRayMask = 0xFF;

//=========================================================================================
// global buffers
//=========================================================================================

RWTexture2D<float4> gReflectionAlbedo   : register(u0, space1);
RWTexture2D<float4> gReflectionNormal   : register(u1, space1);
RWTexture2D<float4> gReflectionMaterial : register(u2, space1);
RWTexture2D<float4> gReflectionPosition : register(u3, space1);

//* scene
RaytracingAccelerationStructure gScene : register(t0, space1);

//* g-buffer
Texture2D<float4> gPosition : register(t1, space1);
Texture2D<float4> gNormal   : register(t2, space1);
Texture2D<float> gDepth     : register(t3, space1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// Payload structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float3 albedo;
	float3 normal;
	float3 position;
	float3 material;
	bool isHit;
	
	//=========================================================================================
	// public methods
	//=========================================================================================
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

Payload TracePrimaryRay(RayDesc desc, uint flag = kFlag) {
	Payload payload = (Payload)0;
	
	TraceRay(gScene, flag, kRayMask, 0, 1, 0, desc, payload);

	return payload;
}
