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

	float TraceShadow(RayDesc desc, RaytracingAccelerationStructure scene) {
#ifdef _INLINE_RAYTRACING
		if (strength <= 0.0f) {
			return 1.0f;
		}
		
		RayQuery<0> q;

		q.TraceRayInline(
			scene,
			flag,
			0xFF,
			desc
		);

		if (q.Proceed()) {
			return 1.0f - strength;
		}
#endif
		return 1.0f;
	}
	
};
