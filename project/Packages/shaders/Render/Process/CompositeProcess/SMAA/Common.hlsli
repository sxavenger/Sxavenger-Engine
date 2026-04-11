#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Process.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

SamplerState gLinearSampler : register(s0);
SamplerState gPointSampler : register(s1);

//* SMAA include
#define SMAALINEARSAMPLER gLinearSampler
#define SMAAPOINTSAMPLER gPointSampler
#define SMAA_HLSL_4_1
#define SMAA_PRESET_ULTRA
#define SMAA_RT_METRICS float4(1.0f / dimension.x, 1.0f / dimension.y, dimension.x, dimension.y)
#include "../../../../Externals/SMAA/SMAA.hlsli"

Texture2D<float4> gAreaTexture  : register(t10);
Texture2D<float4> gSearchTexture : register(t11);
