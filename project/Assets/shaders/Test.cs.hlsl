//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include "../../Packages/shaders/Library/Random.hlsli"
#include "../../Packages/shaders/Library/Hammersley.hlsli"
#include "../../Packages/shaders/Library/ImportanceSample.hlsli"

//* component
#include "../../Packages/shaders/Component/CameraComponent.hlsli"

//* common
#include "../../Packages/shaders/Render/DeferredBufferIndex.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RaytracingAccelerationStructure gScene : register(t0);

Texture2D<float4> gDirect   : register(t1);

RWTexture2D<float4> gOutput : register(u0);

ConstantBuffer<DeferredBufferIndexConstantBuffer> gDeferredBufferIndex : register(b0);
ConstantBuffer<CameraComponent> gCamera : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// Payload structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Payload {

	//=========================================================================================
	// public variables
	//=========================================================================================

	bool isHit;

	bool isFrustum;
	uint2 index;
	float depth;
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint2 GetDimension() {
	uint width, height;
	gDirect.GetDimensions(width, height);
	return uint2(width, height);
}

Payload TracePrimaryRay(RayDesc desc) {
	RayQuery<0> query;

	static const uint kFlag    = RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
	static const uint kRayMask = 0xFF;

	query.TraceRayInline(
		gScene,
		kFlag,
		kRayMask,
		desc
	);

	Payload payload;
	payload.isHit = query.Proceed();

	if (!payload.isHit) {
		//!< ヒットしなかった
		return payload;
	}

	float3 position = desc.Origin + desc.Direction * query.CandidateTriangleRayT();

	float4 viewproj = mul(float4(position, 1.0f), gCamera.GetViewProj());
	viewproj.xyz /= viewproj.w;
	viewproj.y   *= -1; //!< y軸反転

	//!< スクリーンスペース外判定

	if (viewproj.z < 0.0f || viewproj.z > 1.0f) {
		payload.isFrustum = false;
		return payload;
	}

	if (any(viewproj.xy < -1.0f) || any(viewproj.xy > 1.0f)) {
		payload.isFrustum = false;
		return payload;
	}

	payload.isFrustum = true;

	float2 uv = viewproj.xy * 0.5f + 0.5f;

	payload.index = uint2(uv * GetDimension());
	payload.depth = viewproj.z;

	return payload;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(32, 32, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	//!< SSAOのデモ

	uint2 index = dispatchThreadId.xy;
	uint2 atlas = index * 16;

	if (any(index >= GetDimension())) {
		return; //!< 範囲外
	}

	uint2 offset = Xorshift::xorshift32(index.x * index.y);

	DeferredSurface surface;
	if (!surface.GetSurface(gDeferredBufferIndex.Get(), index)) {
		gOutput[atlas] = float4(0.0f, 0.0f, 0.0f, 1.0f);
		return; //!< surfaceが存在しない
	}

	static const float kTMin = 0.001f;
	static const float kTMax = 10000.0f;

	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = reflect(normalize(gCamera.GetPosition() - surface.position), surface.normal); //!< 仮で反射方向に飛ばす
	desc.TMin      = kTMin;
	desc.TMax      = kTMax;

	Payload payload = TracePrimaryRay(desc);

	if (!payload.isHit) {
		gOutput[atlas] = float4(1.0f, 0.0f, 0.0f, 1.0f);
		return; //!< ヒットしなかった
	}

	if (!payload.isFrustum) {
		gOutput[atlas] = float4(0.0f, 1.0f, 0.0f, 1.0f);
		return; //!< フラスタム外
	}

	DeferredSurface hit_surface;
	if (!hit_surface.GetSurface(gDeferredBufferIndex.Get(), payload.index)) {
		gOutput[atlas] = float4(0.0f, 0.0f, 0.0f, 1.0f);
		return; //!< surfaceが存在しない(そんなことはないはず)
	}

	if (hit_surface.depth > payload.depth + 0.0001f) {
		gOutput[atlas] = float4(0.0f, 0.0f, 1.0f, 1.0f);
		return; //!< 奥にある
	}

	float3 hit_direct = gDirect.Load(uint3(payload.index, 0)).rgb;
	gOutput[atlas] = float4(hit_direct, 1.0f);
	
}
