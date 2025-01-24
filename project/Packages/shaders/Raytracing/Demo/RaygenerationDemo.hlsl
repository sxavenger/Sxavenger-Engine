//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RaytracingDemo.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("raygeneration")]
void mainRaygeneration() {
	
	uint2 index = DispatchRaysIndex().xy;
	
	Payload payload = (Payload)0;
	
	RayDesc desc;
	desc.Origin    = float3(0.0f, 0.0f, 0.0f);
	desc.Direction = normalize(float3(0.0f, 0.0f, 1.0f));
	desc.TMin      = kDefaultTMin;
	desc.TMax      = kDefaultTMax;

	TraceRay(desc, payload);

	gOutput[index] = float4(payload.color, 1.0f);
	
}
