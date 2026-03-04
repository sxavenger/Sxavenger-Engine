#pragma once

#ifndef _MATHMATIC_LIBRAY
#define _MATHMATIC_LIBRAY

////////////////////////////////////////////////////////////////////////////////////////////
// Mathmatic namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Mathmatic {

	////////////////////////////////////////////////////////////////////////////////////////////
	// constants
	////////////////////////////////////////////////////////////////////////////////////////////

	static const float kPi = 3.141592653589793f;
	static const float kTau = kPi * 2.0f;

	static const float kPiInv = 1.0f / kPi;
	static const float kTauInv = 1.0f / kTau;

	static const float kEpsilon = 1.192092896e-07f;

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	float Square(float value) {
		return value * value;
	}

	float3 GetPosition(float4x4 mat) {
		return mat[3].xyz;
	}

	float3 GetForwardDirection(float4x4 mat) {
		static const float3 forward = float3(0.0f, 0.0f, 1.0f);
		return normalize(mul(forward, (float3x3)mat));
	}

	float3 GetUpDirection(float4x4 mat) {
		static const float3 up = float3(0.0f, 1.0f, 0.0f);
		return normalize(mul(up, (float3x3)mat));
	}

	float3 GetRightDirection(float4x4 mat) {
		static const float3 right = float3(1.0f, 0.0f, 0.0f);
		return normalize(mul(right, (float3x3)mat));
	}
	
}
#endif
