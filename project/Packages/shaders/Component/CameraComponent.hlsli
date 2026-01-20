#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// CameraComponent structure
////////////////////////////////////////////////////////////////////////////////////////////
struct CameraComponent {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float4x4 view;
	float4x4 world;
	float4x4 proj;
	float4x4 projInv;
	float near;
	float far;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float3 GetPosition() {
		return (float3)world[3];
	}
	
	float4x4 GetViewProj() {
		return mul(view, proj);
	}

	float3 CalculateNDCPosition(float3 position) {
		float4 clip = mul(float4(position, 1.0f), GetViewProj());
		clip /= clip.w; //!< perspective divide
		return clip.xyz;
	}

	float3 GetDirection() {
		static const float3 forward = float3(0.0f, 0.0f, 1.0f);
		return normalize(mul(forward, (float3x3)world));
	}

	float3 GetDirection(float2 uv) {
		float3 target = mul(float4(uv, 1.0f, 1.0f), projInv).xyz;
		return normalize(mul(target, (float3x3)world));
	}
	
};
