//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ReflectionRaytracing.hlsli"
#include "../../Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================
//* local buffer

Texture2D<float4> gDepth    : register(t0);
Texture2D<float4> gNormal   : register(t1);
Texture2D<float4> gPosition : register(t2);
ConstantBuffer<Camera> gCamera : register(b0);

RWTexture2D<float4> gReflection : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[shader("raygeneration")]
void mainRaygen() {

	uint2 index = DispatchRaysIndex().xy;
	uint2 size  = DispatchRaysDimensions().xy;
	
	if (gDepth.Load(uint3(index, 0)) == 1.0f) {
		gReflection[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return;
	}
	
	float3 position_surface = gPosition.Load(uint3(index, 0)).xyz;
	float3 normal_surface   = gNormal.Load(uint3(index, 0)).xyz;
	
	float3 position_camera  = gCamera.GetPosition();
	
	float3 v = normalize(position_camera - position_surface);
	
	//* surfaceÇÃîΩéÀrayÇê∂ê¨
	RayDesc desc;
	desc.Origin    = position_surface;
	desc.Direction = reflect(-v, normal_surface);
	desc.TMin      = kDefaultTMin;
	desc.TMax      = kDefaultTMax;
	
	Payload payload = (Payload)0;
	
	TraceRay(desc, payload);
	
	gReflection[index] = float4(payload.color, 1.0f);
	
}
