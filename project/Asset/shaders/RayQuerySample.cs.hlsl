//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Packages/shaders/Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RaytracingAccelerationStructure gScene : register(t0);
RWTexture2D<float4> gOutput : register(u0);
ConstantBuffer<Camera> gCamera : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// static variables
////////////////////////////////////////////////////////////////////////////////////////////

static const uint2 kImageSize = uint2(1280, 720);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	uint2 size  = kImageSize;

	if (any(index >= size)) {
		return;
	}
	
	RayQuery<RAY_FLAG_NONE> q;
	
	float2 viewport = float2(index) / float2(size) * 2.0f - 1.0f;
	float3 target = mul(float4(viewport.x, -viewport.y, 1.0f, 1.0f), gCamera.projInv).xyz;
	float3 direction = mul(float4(target.xyz, 0.0f), gCamera.world).xyz;
	
	// レイの定義
	RayDesc ray;
	ray.Origin = float3(0, 0, -5);
	ray.Direction = direction;
	ray.TMax = 10000.0;
	ray.TMin = 0.001;
	
	// レイトレーシングの実行
	q.TraceRayInline(
		gScene, // アクセラレーション構造
		RAY_FLAG_NONE, // レイフラグ
		0xFF, // インスタンスマスク
		ray // レイの説明
	);
	
	// 結果の処理
	if (q.Proceed()) {
		gOutput[index] = float4(1, 0, 0, 1);
		
	} else {
		gOutput[index] = float4(0, 0, 0, 1);

	}
	
}
