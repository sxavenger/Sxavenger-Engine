#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RestirCommon.hlsli"

//* library
#include "../../Library/ImportanceSample.hlsli"

//* content
#include "../../Content/Random.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"
#include "../../Component/LightComponentCommon.hlsli"
#include "../../Component/DirectionalLightComponent.hlsli"
#include "../../Component/PointLightComponent.hlsli"
#include "../../Component/SpotLightComponent.hlsli"
#include "../../Component/SkyLightComponent.hlsli"
#include "../../Component/SkyAtmosphereComponent.hlsli"

//* common
#include "../DeferredBufferIndex.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

//* shaders
#define _RAYGENERATION [shader("raygeneration")]
#define _ANYHIT        [shader("anyhit")]
#define _CLOSESTHIT    [shader("closesthit")]
#define _MISS          [shader("miss")]

////////////////////////////////////////////////////////////////////////////////////////////
// Config variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kTMin = 0.001f;
static const float kTMax = 10000.0f;

static const uint kFlag    = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
static const uint kRayMask = 0xFF;

static const uint kMaxRecursionDepth = 3;

//=========================================================================================
// global buffers
//=========================================================================================

RWStructuredBuffer<Reservoir> gInitalizeReservoir : register(u0, space1);
RWStructuredBuffer<Moment> gMoment                : register(u1, space1);
//!< array size [DispatchRaysDimensions().x * DispatchRaysDimensions().y]

//* scene
RaytracingAccelerationStructure gScene : register(t0, space1);

//* deferred index
ConstantBuffer<DeferredBufferIndexConstantBuffer> gDeferredBufferIndex : register(b0, space1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b1, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// Config cbuffer 32bitconstants.
////////////////////////////////////////////////////////////////////////////////////////////
cbuffer Config : register(b2, space1) {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	uint maxSampleCount;
	uint samplesPerFrame;
	
};

cbuffer Seed : register(b3, space1) {
	uint3 seed;
}

//* light
struct LightCount {
	uint count;
};

// Directional Light
ConstantBuffer<LightCount> gDirectionalLightCount                : register(b0, space2);
StructuredBuffer<TransformComponent> gDirectionalLightTransforms : register(t0, space2);
StructuredBuffer<DirectionalLightComponent> gDirectionalLights   : register(t1, space2);

// Point Light
ConstantBuffer<LightCount> gPointLightCount                : register(b1, space2);
StructuredBuffer<TransformComponent> gPointLightTransforms : register(t2, space2);
StructuredBuffer<PointLightComponent> gPointLights         : register(t3, space2);

// Spot Light
ConstantBuffer<LightCount> gSpotLightCount                : register(b2, space2);
StructuredBuffer<TransformComponent> gSpotLightTransforms : register(t4, space2);
StructuredBuffer<SpotLightComponent> gSpotLights          : register(t5, space2);

// Sky Light
ConstantBuffer<SkyLightComponent> gSkyLight : register(b3, space2);

SamplerState gSkySampler : register(s0, space2);
//!< TODO: Direct SkyLight に切り替える

////////////////////////////////////////////////////////////////////////////////////////////
// Payload structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint count;
	float3 lo;
	float3 position;
	uint pad;
	float3 normal;
	
	//=========================================================================================
	// public methods
	//=========================================================================================

	void Reset() {
		count    = 0;
		lo       = float3(0.0f, 0.0f, 0.0f);
		position = float3(0.0f, 0.0f, 0.0f);
		normal   = float3(0.0f, 0.0f, 0.0f);
	}

	//* recursion methods *//

	uint GetNextRecursionCount() {
		return count + 1;
	}

	bool CheckNextRecursion() {
		return GetNextRecursionCount() < kMaxRecursionDepth;
	}

	void IncrimentRecursionCount() {
		count++;
	}

	//* recursion ray methods *//

	bool TraceRecursionRay(inout Payload recursion, RayDesc desc, uint flag = kFlag) {
		if (!CheckNextRecursion()) {
			return false;
		}

		recursion.count = GetNextRecursionCount();
		TraceRay(gScene, flag, kRayMask, 0, 1, 0, desc, recursion);

		return true;
	}
	
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

Payload TracePrimaryRay(RayDesc desc, uint flag = kFlag) {
	Payload payload;
	payload.Reset();
	payload.IncrimentRecursionCount();
	
	TraceRay(gScene, flag, kRayMask, 0, 1, 0, desc, payload);

	return payload;
}

