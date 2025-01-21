//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float>  gDepth    : register(t0);
Texture2D<float4> gNormal   : register(t1);
Texture2D<float4> gPosition : register(t2);

RWTexture2D<float4> gOutput : register(u0);

struct Parameter {
	uint2 size;
};
ConstantBuffer<Parameter> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// static variables
////////////////////////////////////////////////////////////////////////////////////////////

static const uint kSampleCount = 16;
static const float pi = 3.14159265359;
static const float radius = 0.1f;
static const float bias = 0.01f;

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float PseudoRandom(float2 uv) {
	uv = frac(uv * float2(12.9898, 78.233)); // 座標に基づく処理
	float dotProduct = dot(uv, float2(43758.5453, 23421.6311));
	return frac(sin(dotProduct) * 43758.5453); // サイン波を使った乱数
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	uint2 size  = gParameter.size;
	
	if (any(index >= size)) {
		gOutput[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return;
	}
	
	// データの取得
	float depth = gDepth.Load(uint3(index, 0)).r;

	if (depth >= 1.0f) {
		gOutput[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return;
	}
	
	float3 position = gPosition.Load(uint3(index, 0)).rgb;
	float3 normal   = gNormal.Load(uint3(index, 0)).rgb * 2.0f - 1.0f;
	
	float2 uv = index / size;
	
	// サンプリング処理
	uint count = 0;
	
	for (uint i = 0; i < kSampleCount; i++) {
		float angle   = PseudoRandom(uv + i * 0.1f) * 2.0 * pi; // random angle
		float3 random = float3(cos(angle), sin(angle), PseudoRandom(uv + i * 0.2));
		random        = normalize(random + normal); // fixed random direction

		// サンプリング位置
		float3 samplePos = position + random * radius;

		// サンプリング位置のDepth取得
		float2 sampleUV   = (samplePos.xy / samplePos.z) * 0.5 + 0.5; // サンプル位置をUVに変換
		float sampleDepth = gDepth[sampleUV].r;

		// 奥行き差を評価
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(depth - sampleDepth));
		count += (sampleDepth < samplePos.z + bias) ? 1 : 0;

	}

	// 出力
	float occlusion = 1.0 - (count / kSampleCount);
	gOutput[index] = float4(occlusion, occlusion, occlusion, 1.0f);
	
}
