//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RaytracingDemo.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("raygeneration")]
void mainRaygeneration() {
	
	Payload payload = (Payload)0;
	
	RayDesc desc;
	desc.Origin    = float3(0, 0, 0);
	desc.Direction = normalize(float3(0, 0, 1));
	desc.TMin      = kDefaultTMin;
	desc.TMax      = kDefaultTMax;

	TraceRay(desc, payload);
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// miss main TEST
////////////////////////////////////////////////////////////////////////////////////////////
[shader("miss")]
void mainMiss(inout Payload payload) {
	payload = (Payload)0;

}
