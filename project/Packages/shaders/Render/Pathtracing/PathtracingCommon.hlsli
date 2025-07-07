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
#include "../../Component/PointLightComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"

//=========================================================================================
// global buffers
//=========================================================================================

//* pathtracing output textures.
RWTexture2D<float4> gMain        : register(u0, space1);
RWTexture2D<float4> gNormal      : register(u1, space1);
RWTexture2D<float4> gMaterialARM : register(u2, space1);
RWTexture2D<float>  gDepth       : register(u3, space1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0, space1);
static const float4x4 kViewProj = gCamera.GetViewProj();

//* scene
RaytracingAccelerationStructure gScene : register(t0, space1);

//* pathtracing common
struct Reservoir {
	uint sampleCount;
	uint frameSampleCount;
	uint currentFrame;

	bool CheckNeedSample() {
		return frameSampleCount * currentFrame < sampleCount;
	}
	
};
ConstantBuffer<Reservoir> gReservoir : register(b1, space1);

//* light
struct LightCount {
	uint count;
};

// Directional Light
ConstantBuffer<LightCount> gDirectionalLightCount                : register(b2, space1);
StructuredBuffer<TransformComponent> gDirectionalLightTransforms : register(t1, space1);
StructuredBuffer<DirectionalLightComponent> gDirectionalLights   : register(t2, space1);
StructuredBuffer<InlineShadow> gDirectionalLightShadows          : register(t3, space1);

// Point Light
ConstantBuffer<LightCount> gPointLightCount                : register(b3, space1);
StructuredBuffer<TransformComponent> gPointLightTransforms : register(t4, space1);
StructuredBuffer<PointLightComponent> gPointLights         : register(t5, space1);
StructuredBuffer<InlineShadow> gPointLightShadows          : register(t6, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// config variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kTMin = 0.001f;   // minimum t
static const float kTMax = 16384.0f; // maximum t

static const uint kFlag    = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
static const uint kRayMask = 0xFF;

static const uint kMaxRecursionDepth = 3;

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

	//* ray type methods *//

	bool IsPrimary() {
		return rayType == RayType::kPrimary;
	}

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

	bool CheckNextRecursion() {
		return GetNextRecursionCount() < kMaxRecursionDepth;
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
		if (!CheckNextRecursion()) {
			return;
		}

		payload.count = GetNextRecursionCount();
		
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

RayDesc GetPrimaryRayDesc(uint2 index, uint2 dimension) {
	RayDesc desc;
	desc.Origin = gCamera.GetPosition();
	
	float2 d       = (index.xy + 0.5f) / dimension.xy * 2.0f - 1.0f;
	float3 target  = mul(float4(d.x, -d.y, 1.0f, 1.0f), gCamera.projInv).xyz;
	desc.Direction = mul(float4(target, 0.0f), gCamera.world).xyz;
	
	desc.TMin = kTMin;
	desc.TMax = kTMax;
	
	return desc;
}
