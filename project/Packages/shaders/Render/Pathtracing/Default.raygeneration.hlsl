//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PathtracingCommon.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_RAYGENERATION void mainRaygeneration() {

	uint2 index     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	RayDesc desc    = GetPrimaryRayDesc(index, dimension);
	Payload payload = TracePrimaryRay(desc);

	if (gReservoir.IsBeginFrame()) {
		gMain[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	// texture reset
	gPosition[index]    = float4(payload.position, 1.0f);
	gNormal[index]      = float4(payload.normal, 1.0f);
	gMaterialARM[index] = float4(payload.arm, 1.0f);
	gDepth[index]       = payload.depth;
	gMain[index]        += payload.color;
	
}
