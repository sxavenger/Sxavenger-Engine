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
#include "../../Component/LightComponentCommon.hlsli"
#include "../../Component/DirectionalLightComponent.hlsli"
#include "../../Component/PointLightComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"

//=========================================================================================
// global buffers
//=========================================================================================

//* lighting textures
RWTexture2D<float4> gIndirect : register(u0, space1);

//* scene
RaytracingAccelerationStructure gScene : register(t0, space1);

//* deferred textures
Texture2D<float4> gAlbedo      : register(t1, space1);
Texture2D<float4> gNormal      : register(t2, space1);
Texture2D<float4> gMaterialARM : register(t3, space1);
Texture2D<float4> gPosition    : register(t4, space1);
Texture2D<float>  gDepth       : register(t5, space1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// Reservoir cbuffer 32bitconstants.
////////////////////////////////////////////////////////////////////////////////////////////
cbuffer Reservoir : register(b1, space1) {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	uint sampleCount;
	
	uint sampleStep;
	uint currentFrame;

	//=========================================================================================
	// public methods
	//=========================================================================================

	bool CheckNeedSample() {
		return sampleStep * currentFrame < sampleCount;
	}

	bool IsBeginFrame() {
		return currentFrame == 0;
	}
	
};

//* light
struct LightCount {
	uint count;
};

// Directional Light
ConstantBuffer<LightCount> gDirectionalLightCount                : register(b2, space2);
StructuredBuffer<TransformComponent> gDirectionalLightTransforms : register(t1, space2);
StructuredBuffer<DirectionalLightComponent> gDirectionalLights   : register(t2, space2);
StructuredBuffer<InlineShadow> gDirectionalLightShadows          : register(t3, space2);

// Point Light
ConstantBuffer<LightCount> gPointLightCount                : register(b3, space2);
StructuredBuffer<TransformComponent> gPointLightTransforms : register(t4, space2);
StructuredBuffer<PointLightComponent> gPointLights         : register(t5, space2);
StructuredBuffer<InlineShadow> gPointLightShadows          : register(t6, space2);

////////////////////////////////////////////////////////////////////////////////////////////
// Config variables
////////////////////////////////////////////////////////////////////////////////////////////


static const float kTMin = 0.001f;
static const float kTMax = 16384.0f;

static const uint kFlag    = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
static const uint kRayMask = 0xFF;

static const uint kMaxRecursionDepth = 3;

////////////////////////////////////////////////////////////////////////////////////////////
// DeferredSurface structure
////////////////////////////////////////////////////////////////////////////////////////////
struct DeferredSurface {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float depth;
	float3 albedo;
	float3 normal;
	float3 position;
	
	float roughness;
	float metallic;

	//=========================================================================================
	// public methods
	//=========================================================================================

	bool GetSurface(uint2 index) {

		depth = gDepth[index];

		if (depth == 1.0f) {
			return false;
		}

		albedo   = gAlbedo[index].rgb;
		normal   = normalize(gNormal[index].rgb * 2.0f - 1.0f);
		position = gPosition[index].xyz;

		float3 material = gMaterialARM[index].rgb;
		// r channel: ambient
		roughness = material.g;
		metallic  = material.b;

		return true;
	}
	
};

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
