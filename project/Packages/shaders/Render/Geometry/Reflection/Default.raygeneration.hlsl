//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ReflectionCommon.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
RAYGENERATION void mainRaygeneration() {

	uint2 index     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	if (gDepth.Load(int3(index, 0)) == 1.0f) { //!< depthが1.0fの場合は何もせず終了
		gReflection[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	float2 uv = float2(index + 0.5f) / float2(dimension) * 2.0f - 1.0f;
	float3 direction = gCamera.GetDirection(float2(uv.x, -uv.y));

	float3 normal = gNormal.Load(int3(index, 0)).xyz * 2.0f - 1.0f;

	RayDesc desc;
	desc.Direction = reflect(-direction, normal);
	desc.Origin    = gPosition.Load(int3(index, 0)).xyz;
	desc.TMin      = kTMin;
	desc.TMax      = kTMax;

	Payload payload = TracePrimaryRay(desc, kFlag);

	float3 color = payload.color;

	gReflection[index] = float4(payload.color, payload.isHit);
	
}
