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
RWTexture2D<float4> gMain        : register(u0, space1);
RWTexture2D<float4> gNormal      : register(u1, space1);
RWTexture2D<float4> gMaterialARM : register(u2, space1);
RWTexture2D<float>  gDepth       : register(u3, space1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0, space1);
static const float4x4 kViewProj = gCamera.GetViewProj();

//* scene
RaytracingAccelerationStructure gScene : register(t0, space1);

//* light
struct LightCount {
	uint count;
};

// Directional Light
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

static const uint kMaxRecursionDepth = 2;

////////////////////////////////////////////////////////////////////////////////////////////
// RayType enum
////////////////////////////////////////////////////////////////////////////////////////////

namespace RayType {
	static const uint //!< RayType enum type
		kPrimary    = 0, //!< primary ray (ex. view camera from raygeneration ray)
		kReflection = 1; //!< reflection ray
}

////////////////////////////////////////////////////////////////////////////////////////////
// Payload structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {

	//=========================================================================================
	// public variables
	//=========================================================================================

	//* common 
	
	uint rayType; //!< RayType enum type.
	uint count; //!< hit count

	//* primary output variables

	float4 color;
	float3 normal;
	float3 arm; //!< ambient, roughness, metallic
	float depth;

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* recursion methods *//

	void IncrimentRecursionCount() {
		count++;
	}
	
	uint GetNextRecursionCount() {
		return count + 1;
	}

	bool CheckRecursion() {
		return count < kMaxRecursionDepth;
	}

	//* primary ray methods *//

	void SetPrimaryParameter(float3 position, float3 _normal, float ao, float roughness, float metallic) {
		float4 d = mul(float4(position, 1.0f), kViewProj);
		depth = d.z / d.w;

		normal = (_normal + 1.0f) * 0.5f;
		arm    = float3(ao, roughness, metallic);
	}

	//* common methods *//

	void Reset() {
		rayType  = RayType::kPrimary;
		
		count = 0;

		color  = float4(0.0f, 0.0f, 0.0f, 0.0f);
		normal = float3(0.0f, 0.0f, 0.0f);
		arm    = float3(0.0f, 0.0f, 0.0f);
		depth  = 1.0f;
	}

	void TraceRecursionRay(inout Payload payload, RayDesc desc, uint flag = kFlag) {
		payload.count = GetNextRecursionCount();

		if (payload.CheckRecursion()) {
			return;
		}
		
		TraceRay(gScene, flag, kRayMask, 0, 1, 0, desc, payload);
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
