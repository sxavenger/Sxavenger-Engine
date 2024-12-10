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
		return normalize(mul(normal, (float3x3)matInverseTransopse));
	}
};