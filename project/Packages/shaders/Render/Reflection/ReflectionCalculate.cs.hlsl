//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../DeferredBuffers.hlsli"

//* library
#include "../../Library/BRDF.hlsli"
#include "../../Library/ImportanceSample.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

#define NUM_THREADS_X 16
#define NUM_THREADS_Y 16
#define NUM_THREADS_Z 1

//=========================================================================================
// buffers
//=========================================================================================

//* output
RWTexture2D<float4> gReflection : register(u0);

cbuffer Dimension : register(b0) {
	uint2 dimension;
};

//* reflection
Texture2D<float4> gReflectionRadiance : register(t0);
Texture2D<float4> gReflectionPosition : register(t1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, NUM_THREADS_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	Surface surface;
	if (!surface.GetSurface(pixel)) {
		gReflection[pixel] = float4(0.0f, 0.0f, 0.0f, 1.0f);
		return; //!< 取得失敗
	}

	// TODO: Spatialで, 近傍のピクセルから反射光をサンプリングする

	float3 radiance = gReflectionRadiance.Load(uint3(pixel, 0)).rgb;

	float2 uv = (float2(pixel) + 0.5f) / float2(dimension);
	float2 viewport = uv * 2.0f - 1.0f; //!< [-1, 1]の範囲に変換
	viewport.y *= -1.0f;                //!< 上下反転

	float3 v = -gCamera.GetDirection(viewport);
	float3 r = reflect(-v, surface.normal);

	BxDFAlbedo albedo   = BxDFAlbedo::Create(surface.albedo, surface.metallic);
	BxDFContext context = BxDFContext::Create(surface.normal, v, r);

	if (!context.IsValid()) {
		gReflection[pixel] = float4(0.0f, 0.0f, 0.0f, 1.0f);
		return;
	}

	float3 color = EvaluateSpecular(albedo, context, surface.roughness) * radiance;
	// FIXME: Unreal Engineとの結果の差異が大きい. 何かがおかしい.
	// -> Roughnessを考慮した反射ベクトルではない...

	gReflection[pixel] = float4(color, 1.0f);
}
