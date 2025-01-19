#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// TransformationMatrix sturcture
////////////////////////////////////////////////////////////////////////////////////////////
struct TransformationMatrix {
	float4x4 mat;
	float4x4 matInverseTransopse;

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
		static const float3 forward = float3(0.0f, 0.0f, -1.0f);
		return mul(forward, (float3x3)mat);
	}
	
};
