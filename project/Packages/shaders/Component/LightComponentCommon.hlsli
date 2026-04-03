#pragma once

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
		static const float kDefaultTMax = 10000.0f;
		static const float kDefaultTMin = 0.04f;

		RayDesc desc;
		desc.Origin    = origin;
		desc.Direction = direction;
		desc.TMin      = kDefaultTMin;
		desc.TMax      = tmax != 0.0f ? tmax : kDefaultTMax;

		if (strength <= 0.0f) {
			return 1.0f;
		}
		
		RayQuery<0> q;

		q.TraceRayInline(
			scene,
			flag,
			0b01,
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
