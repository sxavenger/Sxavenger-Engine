//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RaytracingCommon.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// entry point raygeneration
////////////////////////////////////////////////////////////////////////////////////////////
[shader("raygeneration")]
void mainRaygeneration() {

	uint2 launchIndex = DispatchRaysIndex().xy;
	float2 dims = float2(DispatchRaysDimensions().xy);

	float2 d = (launchIndex.xy + 0.5f) / dims.xy * 2.0f - 1.0f;

	RayDesc desc;
	desc.Origin = mul(float4(0.0f, 0.0f, 0.0f, 1.0f), gCamera.worldMatrix).xyz; // FIXME: to row maj

	float4 target  = mul(float4(d.x, -d.y, 1, 1), gCamera.projInverseMatrix);
	desc.Direction = mul(float4(target.xyz, 0), gCamera.worldMatrix).xyz;

	desc.TMin = kTmin;
	desc.TMax = kTmax;

	Payload payload = (Payload)0;
	payload.Init(0, RayType::kRayType_Default);

	TraceRay(payload, desc);

	float density = 0.01f;
	float factor  = exp(-density * payload.intersectionT);

	float4 output = payload.color;
	//output.rgb = lerp(payload.color.rgb, float3(0.76f, 0.79f, 0.82f), 1.0f - factor);

	gOutput[launchIndex.xy] = output;
	gDepth[launchIndex.xy]  = payload.depth;
}
