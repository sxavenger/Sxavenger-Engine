//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ClosesthitCommon.hlsli"
#include "../Light.hlsli"
#include "RaytracingPBRLib.hlsli"

//=========================================================================================
// LocalBuffer
//=========================================================================================
//!< t0, t1 は face情報で使用

Texture2D<float4> gAlbedo : register(t2);
SamplerState     gSampler : register(s0);

ConstantBuffer<PBRMaterial> gPBRMaterial : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// static variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float3 kSampleLightDirection = normalize(float3(0.0f, -1.0f, 1.0f));
// HACK: light containerを使用する

//!< TEST USING METHODS
float4 AlphaBlend(float4 base, float4 add) {
	float4 result;
	
	// alpha
	result.a = base.a + add.a * (1.0f - base.a);
	
	// color
	result.rgb = (base.rgb * base.a + add.rgb * add.a * (1.0f - base.a)) / result.a;
	
	return result;
	
}

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

	payload.color = color;
	
}