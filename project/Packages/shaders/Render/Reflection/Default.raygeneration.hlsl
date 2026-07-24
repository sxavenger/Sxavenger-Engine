//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RaytracingCommon.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
RAYGENERATION void mainRaygeneration() {

	uint2 pixel     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	if (gDepth[pixel] == 1.0f) {
		gReflectionRadiance[pixel] = float4(0.0f, 0.0f, 0.0f, 1.0f);
		gReflectionPosition[pixel] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; //!< surfaceが存在しない
	}

	float3 position = gPosition[pixel].rgb;
	float3 normal   = normalize(gNormal[pixel].rgb * 2.0f - 1.0f); //!< [0, 1]の範囲から[-1, 1]の範囲に変換

	float2 uv = (float2(pixel) + 0.5f) / float2(dimension);
	float2 viewport = uv * 2.0f - 1.0f; //!< [-1, 1]の範囲に変換
	viewport.y *= -1.0f;                //!< 上下反転

	float3 direciton = gCamera.GetDirection(viewport);

	//!< 反射の取得
	RayDesc desc;
	desc.Origin    = position;
	desc.Direction = reflect(direciton, normal);
	desc.TMin      = kTMin;
	desc.TMax      = kTMax;

	Payload payload = Payload::TracePrimary(desc, kFlag);

	gReflectionRadiance[pixel] = float4(payload.radiance, 1.0f);
	gReflectionPosition[pixel] = float4(payload.position, payload.isHit);

}
