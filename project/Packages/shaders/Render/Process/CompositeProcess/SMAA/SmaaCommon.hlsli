#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Process.hlsli"

SamplerState gLinearSampler : register(s0);
SamplerState gPointSampler : register(s1);

#define SMAALINEARSAMPLER gLinearSampler
#define SMAAPOINTSAMPLER gPointSampler
#define SMAA_HLSL_4_1
#define SMAA_PRESET_ULTRA
#define SMAA_RT_METRICS float4(1.0f / size.x, 1.0f / size.y, size.x, size.y)
#include "SMAA.hlsli"

Texture2D<float4> gAreaTexture  : register(t10);
Texture2D<float4> gSearchTexture : register(t11);

//static const SMAASamplers kSamplers = {
//	gLinearSampler,
//	gPointSampler
//};
