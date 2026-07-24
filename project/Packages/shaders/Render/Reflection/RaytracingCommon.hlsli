#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../../Library/RaytracingLib.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"
#include "../../Component/DirectionalLightComponent.hlsli"
#include "../../Component/PointLightComponent.hlsli"
#include "../../Component/SpotLightComponent.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// constant variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kTMin = 0.001f;
static const float kTMax = 10000.0f;

static const uint kFlag    = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
static const uint kRayMask = 0xFF;

static const uint kMaxRecursion = 2;

//=========================================================================================
// global buffers
//=========================================================================================

//* output
RWTexture2D<float4> gReflectionRadiance : register(u0, space1);
RWTexture2D<float4> gReflectionPosition : register(u1, space1);

//* scene
RaytracingAccelerationStructure gScene : register(t0, space1);

//* G-Buffer
//Texture2D<float4> gAlbedo   : register(t1, space1);
Texture2D<float> gDepth     : register(t1, space1);
Texture2D<float4> gNormal   : register(t2, space1);
Texture2D<float4> gPosition : register(t3, space1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0, space1);

//* Light
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

////////////////////////////////////////////////////////////////////////////////////////////
// Payload structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PAYLOAD Payload {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float3 radiance : QUALIFIRE_DEFAULT;
	bool   isHit    : QUALIFIRE_DEFAULT;
	float3 position : QUALIFIRE_DEFAULT;

	//=========================================================================================
	// public methods
	//=========================================================================================

	static Payload TracePrimary(RayDesc desc, uint flag = kFlag) {
		Payload payload = (Payload)0;
		TraceRay(gScene, flag, kRayMask, 0, 1, 0, desc, payload);

		return payload;
	}
	
};
