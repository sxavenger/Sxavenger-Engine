//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ClosesthitCommon.hlsli"
#include "../Light.hlsli"

//=========================================================================================
// LocalBuffer
//=========================================================================================
//!< t0, t1 は face情報で使用

Texture2D<float4> gAlbedo : register(t2);
SamplerState     gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// static variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float3 kSampleLightDirection = normalize(float3(0.0f, -1.0f, 1.0f));
// HACK: light containerを使用する

////////////////////////////////////////////////////////////////////////////////////////////
// entry point
////////////////////////////////////////////////////////////////////////////////////////////
[shader("closesthit")]
void ClosesthitBehavior(inout Payload payload, Attribute attribute) {
	// HACK: payload return や alpha 0 の場合は anyhit で処理をする.

	if (payload.Update()) {
		return;
	}

	payload.SetIntersection(true);
	
	if (payload.rayType == RayType::kRayType_Shadow) {
		return; //!< shadow rayなら色情報が不要なのでreturn
	}

	// world情報のvetexを取得
	Vertex vertex = GetHitWorldVertex(attribute);

	// sampling
	float4 color = gAlbedo.SampleLevel(gSampler, vertex.texcoord, 0.0f);

	color.rgb = HalfLambertReflection(vertex.normal, -kSampleLightDirection) * color.rgb;

	// shadow ray を取得
	Payload shadowPayload = (Payload)0;
	shadowPayload.rayType = RayType::kRayType_Shadow;

	RayDesc shadowDesc;
	shadowDesc.Origin    = vertex.position.xyz;
	shadowDesc.Direction = -kSampleLightDirection;
	shadowDesc.TMin      = kTmin;
	shadowDesc.TMax      = kTmax;

	TraceRay(
		shadowPayload, shadowDesc
	);

	// shadowがヒットしてた場合
	if (shadowPayload.isIntersection) {
		color.rgb = (float3)0;
	}

	payload.color = color;
	
}