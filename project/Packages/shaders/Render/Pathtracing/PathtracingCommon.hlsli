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
//* library
#include "../../Library/Hammersley.hlsli"
#include "ImportanceSample.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"
#include "../../Component/LightComponentCommon.hlsli"
#include "../../Component/DirectionalLightComponent.hlsli"
#include "../../Component/PointLightComponent.hlsli"
#include "../../Component/SkyLightComponent.hlsli"

//* common
#include "../DeferredBufferIndex.hlsli"

//=========================================================================================
// global buffers
//=========================================================================================

//* lighting textures
RWTexture2D<float4> gIndirect     : register(u0, space1);
RWTexture2D<uint> gIndirectMoment : register(u1, space1);

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
	uint isResetMoment;

	//=========================================================================================
	// public methods
	//=========================================================================================
	
};

//* light
struct LightCount {
	uint count;
};

// Directional Light
ConstantBuffer<LightCount> gDirectionalLightCount                : register(b0, space2);
StructuredBuffer<TransformComponent> gDirectionalLightTransforms : register(t0, space2);
StructuredBuffer<DirectionalLightComponent> gDirectionalLights   : register(t1, space2);
StructuredBuffer<InlineShadow> gDirectionalLightShadows          : register(t2, space2);

// Point Light
ConstantBuffer<LightCount> gPointLightCount                : register(b1, space2);
StructuredBuffer<TransformComponent> gPointLightTransforms : register(t3, space2);
StructuredBuffer<PointLightComponent> gPointLights         : register(t4, space2);
StructuredBuffer<InlineShadow> gPointLightShadows          : register(t5, space2);

// Sky Light
ConstantBuffer<SkyLightComponent> gSkyLight : register(b2, space2);
SamplerState gSkySampler                    : register(s0, space2);

////////////////////////////////////////////////////////////////////////////////////////////
// Config variables
////////////////////////////////////////////////////////////////////////////////////////////


static const float kTMin = 0.001f;
static const float kTMax = 16384.0f;

static const uint kFlag    = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
static const uint kRayMask = 0xFF;

static const uint kMaxRecursionDepth = 3;

////////////////////////////////////////////////////////////////////////////////////////////
// Payload structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* ray primary

	uint count;

	//* parameter

	float4 indirect;
	
	//=========================================================================================
	// public methods
	//=========================================================================================

	void Reset() {
		count    = 0;
		indirect = float4(0.0f, 0.0f, 0.0f, 0.0f);
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
