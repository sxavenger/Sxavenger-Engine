#pragma once

/*
# reference
 - https://bruop.github.io/exposure/
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Process.hlsli"

//=========================================================================================
// define
//=========================================================================================

#define _GROUP_SIZE 128

//=========================================================================================
// buffer
//=========================================================================================

Texture2D<float4> gInput    : register(t0); //!< input texture
RWTexture2D<float4> gOutput : register(u0); //!< output texture

RWStructuredBuffer<uint> gHistogram       : register(u1); //!< histogram buffer
RWStructuredBuffer<uint> gHistogramShared : register(u2); //!< histogram shared buffer

RWStructuredBuffer<float> gAverageLuminance : register(u3); //!< average exposure buffer

struct Parameter {
	float minLogLuminance;
	float maxLogLuminance;
	float timeCoeff;
	float compensation;
};
ConstantBuffer<Parameter> gParameter : register(b0); //!< parameter buffer

//=========================================================================================
// constant
//=========================================================================================

static const float kLogLuminanceRange    = gParameter.maxLogLuminance - gParameter.minLogLuminance;
static const float kLogLuminanceRangeInv = 1.0f / kLogLuminanceRange;
