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
RWTexture2DArray<float4> gIrrandiance : register(u0); //!< irradiance(diffuse) environment cube map.

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
  - https://qiita.com/emadurandal/items/b2ae09c5cc1b3da821c8
*/
//!< todo: 文献などを参照し, 関数実装.

float3 ImportanceSampleCosineWieghted(float2 xi, float3 n) {
	float r   = sqrt(xi.x);
	float phi = kTau * xi.y;

	float3 h = float3(r * cos(phi), r * sin(phi), sqrt(1.0f - xi.x)); //!< 半径1の円周上にサンプリング

	float3 up       = abs(n.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f); //!< 上方向を決定
	float3 tangentX = normalize(cross(up, n)); //!< 接線ベクトル
	float3 tangentY = cross(n, tangentX); //!< 接線ベクトル

	return normalize(h.x * tangentX + h.y * tangentY + h.z * n); //!< 法線ベクトルに変換
}

float3 PrefilterIrradiance(float3 n) {

	float3 color = 0.0f;

	for (uint i = 0; i < kSampleCount; i++) {
		float2 xi = Hammersley(i, kSampleCount);
		float3 h  = ImportanceSampleCosineWieghted(xi, n); //!< 半径1の円周上にサンプリング

		color += gEnvironment.SampleLevel(gEnvironmentSampler, h, 0.0f).rgb; //!< 環境マップから色を取得
	}

	return color / float(kSampleCount); //!< 平均化
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint3 size;
	gIrrandiance.GetDimensions(size.x, size.y, size.z);

	if (any(dispatchThreadId >= size)) {
		return; //!< out of range.
	}

	uint3 index = dispatchThreadId;
	float2 uv   = (float2(dispatchThreadId.xy + 0.5f) / size.xy) * 2.0f - 1.0f; //!< [-1, 1]に変換

	float3 direction = GetDirection(uv, index.z); //!< Normalとして扱ってみる.

	gIrrandiance[index] = float4(PrefilterIrradiance(direction), 1.0f); //!< 環境マップから色を取得
	
}
