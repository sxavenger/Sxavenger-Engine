//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ClosesthitCommon.hlsli"
#include "../Light.hlsli"
#include "RaytracingPBRLib.hlsli"
#include "../ColorUtils.hlsli"

//=========================================================================================
// LocalBuffer
//=========================================================================================
//!< t0, t1 は face情報で使用. b0はUVで使用

Texture2D<float4> gAlbedo : register(t2);
SamplerState     gSampler : register(s0);

ConstantBuffer<PBRMaterial> gPBRMaterial : register(b1);

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

	if (payload.UpdateIntersection(true)) {
		return;
	}

	// world情報のvetexを取得
	Vertex vertex = GetHitWorldVertex(attribute);

	// sampling
	float4 color = gAlbedo.SampleLevel(gSampler, vertex.texcoord, 0.0f);

	//* ライティング処理 *//

	//* PBRライティング
	//* 反射したときの色の取得
	Payload reflectionPayload = (Payload)0;
	reflectionPayload.Init(payload.intersectionCount);

	RayDesc reflectionDesc;
	reflectionDesc.Origin    = vertex.position.xyz;
	reflectionDesc.Direction = reflect(WorldRayDirection(), vertex.normal);
	reflectionDesc.TMax      = kTmax;
	reflectionDesc.TMin      = kTmin;

	TraceRay(
		reflectionPayload, reflectionDesc
	);
	
	PBRMaterial material = gPBRMaterial;

	if (reflectionPayload.color.a == 0.0f) {
		color.rgb = GGX(material, vertex.normal, -WorldRayDirection(), -kSampleLightDirection) * color.rgb;
		
	} else {
		color.rgb = RaytracingGGX(material, vertex.normal, -WorldRayDirection(), -kSampleLightDirection, reflectionPayload.color.rgb) * color.rgb;
	}

	//* 影
	Payload shadowPayload = (Payload)0;
	shadowPayload.Init(payload.intersectionCount, RayType::kRayType_Intersection);

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
		color.rgb = AlphaBlend(float4(0.0f, 0.0f, 0.0f, 0.5f), float4(color.rgb, 1.0f)).rgb;
	}

	payload.SetDepth(vertex.position);
	
	payload.color = max(color, 0.0f);
	
}