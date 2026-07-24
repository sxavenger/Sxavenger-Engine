#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// constant variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float kShadowDefaultTMax = 10000.0f;
static const float kShadowDefaultTMin = 0.02f;
static const uint kShadowMask         = 0b01;

////////////////////////////////////////////////////////////////////////////////////////////
// InlineShadow structure
////////////////////////////////////////////////////////////////////////////////////////////
struct InlineShadow {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float strength;
	uint flag;

	//=========================================================================================
	// public variables
	//=========================================================================================

	float TraceShadow(RaytracingAccelerationStructure scene, float3 origin, float3 direction, float tmax = 0.0f) {
#ifdef _SUPPORT_INLINE_RAYTRACING

		RayDesc desc;
		desc.Origin    = origin;
		desc.Direction = direction;
		desc.TMin      = kShadowDefaultTMin;
		desc.TMax      = tmax != 0.0f ? tmax : kShadowDefaultTMax;

		if (strength <= 0.0f) {
			return 1.0f;
		}
		
		RayQuery<0> q;

		q.TraceRayInline(
			scene,
			flag,
			kShadowMask,
			desc
		);

		if (q.Proceed()) {
			return 1.0f - strength;
		}
#endif

		return 1.0f;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// LightCount structure
////////////////////////////////////////////////////////////////////////////////////////////
struct LightCount {
	
	//=========================================================================================
	// public variables
	//=========================================================================================
	
	uint count;
};
