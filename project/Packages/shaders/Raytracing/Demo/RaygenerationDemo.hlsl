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
	uint2 size  = DispatchRaysDimensions().xy;
	
	float2 viewport  = float2(index) / float2(size) * 2.0f - 1.0f;
	float3 target    = mul(float4(viewport.x, -viewport.y, 1.0f, 1.0f), gCamera.projInv).xyz;
	float3 direction = mul(float4(target.xyz, 0.0f), gCamera.world).xyz;
	
	RayDesc desc;
	desc.Origin    = gCamera.GetPosition();
	desc.Direction = direction;
	desc.TMin      = kDefaultTMin;
	desc.TMax      = kDefaultTMax;
	
	Payload payload = (Payload)0;

	TraceRay(desc, payload);

	gOutput[index] = float4(payload.color, 1.0f);
	
}
