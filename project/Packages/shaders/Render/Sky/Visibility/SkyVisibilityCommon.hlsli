#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Reservoir.hlsli"

//* library
#include "../../../Library/ImportanceSample.hlsli"
#include "../../../Library/ACES.hlsli"

//* content
#include "../../../Content/Random.hlsli"

//* component
#include "../../../Component/CameraComponent.hlsli"

//* gbuffer
#include "../../GBuffer.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

#define NUM_THREAD_X 16
#define NUM_THREAD_Y 16
#define NUM_THREAD_Z 1

////////////////////////////////////////////////////////////////////////////////////////////
// constant variables
////////////////////////////////////////////////////////////////////////////////////////////

static const uint kMaxSampleCount = 32; //!< 最大サンプル数

//=========================================================================================
// buffers
//=========================================================================================

cbuffer Dimension : register(b0, space1) {
	uint2 dimension;
};

ConstantBuffer<CameraComponent> gCamera : register(b1, space1);

cbuffer Seed : register(b2, space1) {
	uint3 seed;
};
