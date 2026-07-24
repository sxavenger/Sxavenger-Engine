#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../Library/Mathmatic.hlsli"

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
		return Mathmatic::GetPosition(mat);
	}

	float3 GetForwardDirection() {
		return Mathmatic::GetForwardDirection(mat);
	}

	float GetMaxScale() {
		return max(length(mat[0].xyz), max(length(mat[1].xyz), length(mat[2].xyz)));
	}

	float4x4 GetMatInverse() {
		return transpose(matInverseTransopse);
	}
	
};
