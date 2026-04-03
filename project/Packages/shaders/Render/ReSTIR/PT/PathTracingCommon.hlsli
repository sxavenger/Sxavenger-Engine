#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ReSTIR
#include "../ReSTIRCommon.hlsli"

//* library
#include "../../../Library/ImportanceSample.hlsli"
#include "../../../Library/RaytracingLib.hlsli"

//* component
#include "../../../Component/CameraComponent.hlsli"
#include "../../../Component/TransformComponent.hlsli"
#include "../../../Component/DirectionalLightComponent.hlsli"
#include "../../../Component/PointLightComponent.hlsli"
#include "../../../Component/SpotLightComponent.hlsli"
#include "../../../Component/SkyLightComponent.hlsli"
#include "../../../Component/SkyAtmosphereComponent.hlsli"

//* deferred buffer
#include "../../DeferredBuffers.hlsli"


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

//* output buffers
RWStructuredBuffer<ReSTIR::Reservoir> gInitalizeReservoir : register(u0, space1);
RWStructuredBuffer<ReSTIR::Moment> gMoment                : register(u1, space1);
//!< array size [DispatchRaysDimensions().x * DispatchRaysDimensions().y]

//* scene
RaytracingAccelerationStructure gScene : register(t0, space1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b1, space1);

////////////////////////////////////////////////////////////////////////////////////////////
// Setting cbuffer 32bitconstants.
////////////////////////////////////////////////////////////////////////////////////////////
cbuffer Setting : register(b2, space1) {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	uint maxSampleCount;
	uint samplesPerFrame;
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// Seed cbuffer 32bitconstants
////////////////////////////////////////////////////////////////////////////////////////////
cbuffer Seed : register(b3, space1) {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	uint3 seed;
	
};

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

