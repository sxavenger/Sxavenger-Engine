#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../PostProcess.hlsli"

//* library
#include "../../../../Library/ACES.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// PostFx structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PostFx {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float brightness;   //!< 輝度 [-1.0 ~ 1.0]
	float saturation;   //!< 飽和度 [-1.0 ~ 1.0]
	float colorfulness; //!< 彩度 [0.0 ~ 1.0]
	float clarity;      //!< 明瞭度 [0.0 ~ 1.0]
	float lumaSharpen;  //!< [0.0 ~ 1.0]
	
	//=========================================================================================
	// public methods
	//=========================================================================================

	float3 GetBrightness(float3 color) {
		return color * (1.0f + brightness);
	}

	float3 GetSaturation(float3 color) {
		float luma = dot(ACES::AP1_RGB2Y, color);
		return lerp(luma.rrr, color, 1.0f + saturation);
	}

	float3 GetColorfulness(float3 color) {
		float luma = dot(ACES::AP1_RGB2Y, color);

		float ma = max(max(color.r, color.g), color.b);
		float mi = min(min(color.r, color.g), color.b);
		float s = ma - mi;
		
		float variance = 1 + colorfulness * (1 - s);
		return lerp(luma.rrr, color, variance);
	}
	
};

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput    : register(t0, space1);
RWTexture2D<float4> gOutput : register(u0, space1);

ConstantBuffer<PostFx> gParameter : register(b0, space1);
