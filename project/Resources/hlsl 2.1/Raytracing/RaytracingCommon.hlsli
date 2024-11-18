#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Camera.hlsli"

//=========================================================================================
// TLAS Buffer
//=========================================================================================

RaytracingAccelerationStructure gScene : register(t10);

//=========================================================================================
// Global buffer
//=========================================================================================

ConstantBuffer<Camera> gCamera : register(b10);
static const float4x4 kViewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

//* output
RWTexture2D<float4> gOutput : register(u10);
RWTexture2D<float>  gDepth  : register(u11);

//=========================================================================================
// Config variables
//=========================================================================================

static const float kTmin = 0.0001f;
static const float kTmax = 8192.0f;

static const uint kLimitIntersectionCount = 4;

//=========================================================================================
// Config structure
//=========================================================================================

namespace RayType {
	static const uint kRayType_Default      = 0,
	                  kRayType_Intersection = 1;
}

struct Payload {
	
	float4 color;
	float depth;
	
	uint rayType;

	//* intersections *//
	uint intersectionCount;
	uint isIntersection; //!< using bool
	float intersectionT;

	//* depth *//


	//* methods *//

	void Init(uint _intersectionCount, uint _rayType = RayType::kRayType_Default) {
		color          = (float4)0;
		depth          = 1.0f;
		
		rayType        = _rayType;
		
		intersectionCount = _intersectionCount;
		isIntersection    = false;
		intersectionT     = 0;
	}

	void SetIntersection(bool _isIntersection) {
		isIntersection = _isIntersection;
		intersectionT  = kTmax;
		
		if (_isIntersection) {
			intersectionT  = RayTCurrent();
		}
	}

	//! @brief payload update.
	//! @retval true  is return true.
	//! @retval false is return false.
	bool Update() {
		if (intersectionCount >= kLimitIntersectionCount) {
			color = (float4)0;
			SetIntersection(false);
			return true;
		}

		intersectionCount++;
		return false;
	}

	bool UpdateIntersection(bool _isIntersection) {
		if (intersectionCount >= kLimitIntersectionCount) {
			color = (float4)0;
			SetIntersection(false);
			return true;
		}

		SetIntersection(_isIntersection);

		if (rayType == RayType::kRayType_Intersection) {
			return true;
		}

		intersectionCount++;
		return false;
	}

	void SetDepth(float4 position) {
		float4 clip = mul(position, kViewProj);
		depth = clip.z / clip.w;
	}
};

struct Attribute {
	float2 barys;
};

struct Vertex {
	float4 position;
	float2 texcoord;
	float3 normal;
};

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

void TraceRay(inout Payload payload, RayDesc desc, uint flag = RAY_FLAG_CULL_BACK_FACING_TRIANGLES) {
	uint rayMask   = 0xFF;

	TraceRay(
		gScene,
		flag,
		rayMask,
		0,
		1,
		0,
		desc,
		payload
	);
}

inline float3 CalcBarycentrics(float2 barys) {
	return float3(
		1.0f - barys.x - barys.y,
		barys.x,
		barys.y
	);
}