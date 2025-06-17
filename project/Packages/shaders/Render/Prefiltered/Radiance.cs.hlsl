//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* perfiltered
#include "Prefiltered.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* output
struct OutputBufferIndex {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	uint index;

	//=========================================================================================
	// public methods
	//=========================================================================================

	RWTexture2DArray<float4> GetRadiance() {
		return ResourceDescriptorHeap[index];
	}
	
};
StructuredBuffer<OutputBufferIndex> gIndices : register(t1);

struct Parameter {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	uint2 textureSize;
	uint miplevels;
};
ConstantBuffer<Parameter> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// constant variables
////////////////////////////////////////////////////////////////////////////////////////////

static const uint kSampleCount = 1024;

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint2 GetMipSize(uint miplevel) {
	return uint2(gParameter.textureSize.x >> miplevel, gParameter.textureSize.y >> miplevel);
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
			color       += NdotL * gEnvironment.SampleLevel(gEnvironmentSampler, l, 0).rgb; //!< 環境マップをサンプリング
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

	//!< dispatchThreadId.z / 6 を書き込むmiplevelとする.
	uint miplevel = dispatchThreadId.z / 6;
	uint face     = dispatchThreadId.z % 6;

	if (miplevel >= gParameter.miplevels) {
		return; //!< miplevel out of range.
	}

	uint2 size  = GetMipSize(miplevel); //!< mipLevelに応じたサイズを取得
	uint2 index = dispatchThreadId.xy;

	if (any(index >= size)) {
		return; //!< texture out of range.
	}

	float2 uv        = (float2(index + 0.5f) / size.xy) * 2.0f - 1.0f; //!< [-1, 1]に変換
	float3 direction = GetDirection(uv, face);

	float roughness = float(miplevel) / float(gParameter.miplevels - 1); //!< roughnessを計算

	float3 color = PrefilterRadiance(roughness, direction); //!< directionを(r = v = n)として扱う
	
	RWTexture2DArray<float4> gRadiance = gIndices[miplevel].GetRadiance(); //!< RWTexture2DArray<float4>を取得
	gRadiance[uint3(index, face)] = float4(color, 1.0f);

}
