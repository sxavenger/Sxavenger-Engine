#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// TransformComponent sturcture
////////////////////////////////////////////////////////////////////////////////////////////
struct TransformComponent {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float4x4 mat;
	float4x4 matInverseTransopse;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float4 Transform(float4 position) {
		return mul(position, mat);
	}

	float3 TransformNormal(float3 normal) {
		return mul(normal, (float3x3)matInverseTransopse);
	}
	
	float3 GetPosition() {
		return mat[3].xyz;
	}

	float3 GetDirection() {
		static const float3 forward = float3(0.0f, 0.0f, 1.0f);
		return normalize(mul(forward, (float3x3)mat));
	}

	float GetMaxScale() {
		return max(length(mat[0].xyz), max(length(mat[1].xyz), length(mat[2].xyz)));
	}
	
};
