#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* Lux
#include "../LuxCommon.hlsli"

//* library
#include "../../../Library/RaytracingLib.hlsli"
#include "../../../Library/ImportanceSample.hlsli"

//* component
#include "../../../Component/CameraComponent.hlsli"
#include "../../../Component/TransformComponent.hlsli"
#include "../../../Component/DirectionalLightComponent.hlsli"
#include "../../../Component/PointLightComponent.hlsli"
#include "../../../Component/SpotLightComponent.hlsli"
#include "../../../Component/SkyLightComponent.hlsli"
#include "../../../Component/SkyAtmosphereComponent.hlsli"

//* GBuffer
#include "../../GBuffer.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Config variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kTMin = 0.02f;
static const float kTMax = 10000.0f;

static const uint kFlag    = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
static const uint kRayMask = 0xFF;

//=========================================================================================
// global buffers
//=========================================================================================

//* output buffers
RWTexture2D<float4> gRadiance : register(u0, space1);

//* scene
RaytracingAccelerationStructure gScene : register(t0, space1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0, space1);

//* setting
ConstantBuffer<Lux::Setting> gSetting : register(b1, space1);

//* light
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
// Resolution cbuffer
////////////////////////////////////////////////////////////////////////////////////////////
cbuffer Resolution : register(b2, space1) {
	uint2 resolution;
};


////////////////////////////////////////////////////////////////////////////////////////////
// Payload structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PAYLOAD Payload {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float3 lo : QUALIFIRE_DEFAULT;
	
	//=========================================================================================
	// public methods
	//=========================================================================================

	static Payload Default() {
		return (Payload)0;
	}

	static Payload TracePrimaryRay(float3 origin, float3 direction, uint flag = kFlag) {

		Payload payload = Payload::Default();

		RayDesc desc;
		desc.Origin    = origin;
		desc.Direction = direction;
		desc.TMin      = kTMin;
		desc.TMax      = kTMax;

		TraceRay(gScene, flag, kRayMask, 0, 1, 0, desc, payload);
		return payload;
	}
	
};
