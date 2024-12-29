#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ProcessCommon.hlsli"

//=========================================================================================
// GBuffers
//=========================================================================================

Texture2D<float4> gAlbedo   : register(t10);
Texture2D<float4> gNormal   : register(t11);
Texture2D<float4> gPosition : register(t12);

//=========================================================================================
// ProcessConfig buffer
//=========================================================================================

ConstantBuffer<ProcessConfig> gConfig : register(b10);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float4 GetAlbedo(uint2 index) {
	return gAlbedo.Load(uint3(index, 0));
}

float3 GetNormal(uint2 index) {
	float3 normal = gNormal.Load(uint3(index, 0)).rgb;
	return normalize(normal * 2.0f - 1.0f);
}

float4 GetPosition(uint2 index) {
	return gPosition.Load(uint3(index, 0));
}
