#pragma enable_d3d12_debug_symbols

//=========================================================================================
// Output buffer
//=========================================================================================
RWTexture2D<float4> gOutput : register(u0);

//=========================================================================================
// TLAS buffer
//=========================================================================================
RaytracingAccelerationStructure gScene : register(t0);

//=========================================================================================
// Config structure
//=========================================================================================
struct Payload {
	float4 color;
};

struct Attribute {
	float2 barys;
};

//=========================================================================================
// miss buffer
//=========================================================================================
//struct Color {
//	float4 color;
//};
//ConstantBuffer<Color> gMissColor : register(b0);

//=========================================================================================
// grobal buffer
//=========================================================================================
struct Camera {
	float4x4 world;
	float4x4 projInv;
};
ConstantBuffer<Camera> gCamera : register(b1);


////////////////////////////////////////////////////////////////////////////////////////////
// entry points
////////////////////////////////////////////////////////////////////////////////////////////

[shader("raygeneration")]
void mainRaygen() {
	
	uint2 launchIndex = DispatchRaysIndex().xy;
	float2 dims = float2(DispatchRaysDimensions().xy);
	
	float2 d = (launchIndex.xy + 0.5f) / dims.xy * 2.0f - 1.0f;
	//float aspect  = dims.x / dims.y;
	
	RayDesc desc;
	desc.Origin = mul(gCamera.world, float4(0, 0, 0, 1)).xyz;
	
	float4 target = mul(gCamera.projInv, float4(d.x, -d.y, 1, 1));
	desc.Direction = mul(gCamera.world, float4(target.xyz, 0)).xyz;
	
	desc.TMin = 0.00001f;
	desc.TMax = 100000;

	Payload payload;
	payload.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	RAY_FLAG flags = RAY_FLAG_NONE;
	uint rayMask = 0xFF;
	
	TraceRay(
		gScene,
		flags,
		rayMask,
		0,
		1,
		0,
		desc,
		payload
	);
	
	gOutput[launchIndex.xy] = payload.color;
	
}

[shader("miss")]
void mainMiss(inout Payload payload) {
	payload.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

}

[shader("closesthit")]
void mainCHS(inout Payload payload, Attribute attribute) {
	payload.color = float4(1.0f, 0.0f, 0.0f, 1.0f);

}