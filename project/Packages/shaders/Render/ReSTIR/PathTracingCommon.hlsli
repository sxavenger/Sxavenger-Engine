#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ReSTIR
#include "ReSTIRCommon.hlsli"

//* library
#include "../../Library/ImportanceSample.hlsli"
#include "../../Library/RaytracingLib.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/TransformComponent.hlsli"
#include "../../Component/DirectionalLightComponent.hlsli"
#include "../../Component/PointLightComponent.hlsli"
#include "../../Component/SpotLightComponent.hlsli"
#include "../../Component/SkyLightComponent.hlsli"
#include "../../Component/SkyAtmosphereComponent.hlsli"

//* deferred buffer
#include "../DeferredBuffers.hlsli"


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

RWStructuredBuffer<Reservoir> gInitalizeReservoir : register(u0, space1);
RWStructuredBuffer<Moment> gMoment                : register(u1, space1);

