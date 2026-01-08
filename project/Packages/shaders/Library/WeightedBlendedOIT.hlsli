#pragma once

/*
 # reference
 - [Weighted Blended Order-Independent Transparency](https://jcgt.org/published/0002/02/09/paper.pdf)
 - [OpenGL / Weighted Blended](https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended)
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Math.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Weighted Blended OIT namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace WeightedBlendedOIT {

	////////////////////////////////////////////////////////////////////////////////////////////
	// common methods
	////////////////////////////////////////////////////////////////////////////////////////////

	//! @brief Calculate weight for Weighted Blended OIT
	//! @param[in] transparency: fragment transparency
	//! @param[in] depth: fragment depth (SV_Position.z)
	//! @param[in] near: camera near plane
	//! @param[in] far: camera far plane
	float CalculateWeight(float transparency, float depth, float near, float far) {
		//!< [Weighted Blended Order-Independent Transparency] Depth Weights Improve Occlusion (Eq.10)

		float d = ((near * far) / -depth - far) / (near - far);
		
		float w = max(1e-2, 3e3 * pow(1.0f - d, 3.0f));
		return transparency * w;
	}

	//! @brief Check if the value is approximately opaque (1.0)
	//! @param[in] v: value to check
	bool IsApproximatelyOpacity(float v) {
		return abs(v - 1.0f) <= (abs(v) < 1.0f ? 1.0f : abs(v)) * kEpsilon;
	}

	//! @brief Calculate the final transparent color from accumulation buffer
	//! @param[in] accumulation: accumulation buffer value
	float3 CalculateTransparentColor(float4 accumulation) {
		return accumulation.rgb / max(accumulation.a, kEpsilon);
	}
	
} 
