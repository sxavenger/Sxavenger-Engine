//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RaytracingCommon.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_RAYGENERATION void mainRaygeneration() {

	uint2 index     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	RayDesc desc;
	desc.Origin = gCamera.GetPosition();

	float2 d       = (index.xy + 0.5f) / dimension.xy * 2.0f - 1.0f;
	float3 target  = mul(float4(d.x, -d.y, 1.0f, 1.0f), gCamera.projInv).xyz;
	desc.Direction = mul(float4(target, 0.0f), gCamera.world).xyz;

	desc.TMin = kTMin;
	desc.TMax = kTMax;

	Payload payload = (Payload)0;

	TraceRay(payload, desc);

	gOutput[index] = payload.color;
	gDepth[index]  = payload.depth;
	
}
