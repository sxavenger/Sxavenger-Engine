#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ProbeCommon.hlsli"

//* library
#include "../../Library/RaytracingLib.hlsli"
#include "../../Library/ImportanceSample.hlsli"
#include "../../Library/ACES.hlsli"

//* content
#include "../../Content/Random.hlsli"

//* component
#include "../../Component/TransformComponent.hlsli"
#include "../../Component/CameraComponent.hlsli"
#include "../../Component/LightComponentCommon.hlsli"
#include "../../Component/DirectionalLightComponent.hlsli"
#include "../../Component/PointLightComponent.hlsli"
#include "../../Component/SpotLightComponent.hlsli"
#include "../../Component/SkyLightComponent.hlsli"

//=========================================================================================
// global buffers
//=========================================================================================

RWTexture2DArray<float4> gProbeSample    : register(u0, space1);
RWTexture2DArray<float2> gProbeReservoir : register(u1, space1);
RWTexture2DArray<uint2> gProbeMoment     : register(u2, space1);
/* Texture Layout
 - [Probe Sample] float3 lo, float pdf
 - [Probe Reservoir] float weight_sum, float w
 - [Probe Moment] uint m, uint sampling_offset,
*/

//* scene
RaytracingAccelerationStructure gScene : register(t0, space1);

ConstantBuffer<ProbeConfig> Config : register(b0, space1);

cbuffer Seed : register(b1, space1) {
	uint3 seed;
}

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b2, space1);

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

////////////////////////////////////////////////////////////////////////////////////////////
// Payload structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float3 lo;
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

Reservoir LoadReservoir(uint2 location, uint slice) {
	uint3 texcoord = uint3(location, slice);

	float4 probe_sample    = gProbeSample.Load(texcoord);
	float2 probe_reservoir = gProbeReservoir.Load(texcoord);
	uint probe_moment      = gProbeMoment.Load(texcoord).x;

	Reservoir reservoir;
	reservoir.sample.lo  = probe_sample.rgb;
	reservoir.sample.pdf = probe_sample.a;
	reservoir.weight_sum = probe_reservoir.x;
	reservoir.w          = probe_reservoir.y;
	reservoir.m          = probe_moment.x;

	return reservoir;
}

void SaveReservoir(uint2 location, uint slice, Reservoir reservoir) {
	uint3 texcoord = uint3(location, slice);

	float4 probe_sample    = float4(reservoir.sample.lo, reservoir.sample.pdf);
	float2 probe_reservoir = float2(reservoir.weight_sum, reservoir.w);
	uint probe_moment      = reservoir.m;

	gProbeSample[texcoord]    = probe_sample;
	gProbeReservoir[texcoord] = probe_reservoir;
	gProbeMoment[texcoord].x  = probe_moment;
}

Payload TracePrimaryRay(float3 origin, float3 direction) {
	Payload payload;
	payload.lo = float3(0.0f, 0.0f, 0.0f);
	
	RayDesc desc;
	desc.Origin    = origin;
	desc.Direction = direction;
	desc.TMin      = 0.01f;
	desc.TMax      = 10000.0f;
	
	TraceRay(gScene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xFF, 0, 1, 0, desc, payload);
	
	return payload;
}
