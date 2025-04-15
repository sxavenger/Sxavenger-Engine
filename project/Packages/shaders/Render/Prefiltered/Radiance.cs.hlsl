//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../../Library/Math.hlsli"
#include "../../Library/Hammersley.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// defines
////////////////////////////////////////////////////////////////////////////////////////////

#define _NUM_THREADS_X 16
#define _NUM_THREADS_Y 16

//=========================================================================================
// buffers
//=========================================================================================

//* input
TextureCube<float4> gEnvironment : register(t0); //!< 環境マップ
SamplerState gEnvironmentSampler : register(s0); //!< 環境マップSampler

//* output
//!< HACK: miplevelが4で固定
RWTexture2DArray<float4> gRadiance0 : register(u0); //!< radiance environment cube map. miplevel 0
RWTexture2DArray<float4> gRadiance1 : register(u1); //!< radiance environment cube map. miplevel 1
RWTexture2DArray<float4> gRadiance2 : register(u2); //!< radiance environment cube map. miplevel 2
RWTexture2DArray<float4> gRadiance3 : register(u3); //!< radiance environment cube map. miplevel 3

////////////////////////////////////////////////////////////////////////////////////////////
// constant variables
////////////////////////////////////////////////////////////////////////////////////////////

static const uint kSampleCount = 1024;

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 GetDirection(float2 uv, uint face) {
	switch (face) {
		case 0:
			return normalize(float3(1.0f, -uv.y, -uv.x)); //!< +x

		case 1:
			return normalize(float3(-1.0f, -uv.y, uv.x)); //!< -x

		case 2:
			return normalize(float3(uv.x, 1.0f, uv.y)); //!< +y

		case 3:
			return normalize(float3(uv.x, -1.0f, -uv.y)); //!< -y

		case 4:
			return normalize(float3(uv.x, -uv.y, 1.0f)); //!< +z

		case 5:
			return normalize(float3(-uv.x, -uv.y, -1.0f)); //!< -z
	}

	return float3(0.0f, 0.0f, 0.0f); //!< error.
}

/*
 # reference
  - https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
  - https://qiita.com/emadurandal/items/b2ae09c5cc1b3da821c8
*/

float3 ImportanceSampleGGX(float2 xi, float roughness, float3 n) {
	float a = roughness * roughness;

	float phi      = kTau * xi.x;
	float cosTheta = sqrt((1.0f - xi.y) / (1.0f + (a * a - 1.0f) * xi.y));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	float3 h = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta); //!< 半径1の円周上にサンプリング

	float3 up = abs(n.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
	float3 tangentX = normalize(cross(up, n)); //!< 接線ベクトル
	float3 tangentY = cross(n, tangentX); //!< 接線ベクトル

	return normalize(h.x * tangentX + h.y * tangentY + h.z * n);
}

float3 PrefilterRadiance(float roughness, float3 r) {
	// 観測者が物体の真上から見下ろしていることを前提(v = n = r);
	float3 n = r;
	float3 v = r;

	float3 color      = 0.0f;
	float totalWeight = 0.0f;
	
	for (uint i = 0; i < kSampleCount; ++i) {
		float2 xi = Hammersley(i, kSampleCount);
		float3 h  = ImportanceSampleGGX(xi, roughness, n); //!< 法線ベクトルをサンプリング
		float3 l  = 2.0f * dot(v, h) * h - v; //!< ライトベクトルを計算

		float NdotL = saturate(dot(n, l)); //!< ライトベクトルと法線ベクトルの内積

		if (NdotL > 0.0f) {
			color       += NdotL * gEnvironment.SampleLevel(gEnvironmentSampler, l, 0.0f).rgb; //!< 環境マップをサンプリング
			totalWeight += NdotL;
		}
	}

	return color / totalWeight;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	
	
}
