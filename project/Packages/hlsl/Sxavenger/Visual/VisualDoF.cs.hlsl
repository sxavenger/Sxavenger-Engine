//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "VisualProcess.hlsli"
#include "../../ColorUtils.hlsli"
#include "../../Camera3d.hlsli"

//=========================================================================================
// Input
//=========================================================================================

Texture2D<float4> gInput : register(t0);
Texture2D<float> gDepth  : register(t1);

ConstantBuffer<Camera3d> gCamera : register(b0);

struct Parameter {
	float focusLength;
	float f;
	uint isDebugDoF;
};
ConstantBuffer<Parameter> gParam : register(b1);

//=========================================================================================
// Output
//=========================================================================================

RWTexture2D<float4> gOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float FixedDepth(float depth) {
	float z = depth;

	// 逆射影行列を使用してビュー空間の位置に戻す
	float4 clipSpacePos = float4(0.0, 0.0, z, 1.0);
	float4 viewSpacePos = mul(clipSpacePos, gCamera.projInverseMatrix);

	// viewSpacePos.z / viewSpacePos.w で線形深度を取得
	return viewSpacePos.z / viewSpacePos.w;
}

float Gaussian2d(float2 diff, float sigma) {
	float squared = dot(diff, diff); //!< x^2 + y^2
	return exp(-squared / (2.0f * sigma * sigma));
}

float4 Sample(uint2 currentIndex, int2 diff) {
	int2 texIndex = int2(currentIndex) + diff;
	texIndex = clamp(texIndex, float2(0.0f, 0.0f), gConfig.size - 1.0f);
	return gInput[texIndex];
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUMTHREADS_X, _NUMTHREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 currentId = dispatchThreadId.xy;
	uint2 size      = gConfig.size;

	// 画像サイズ以上の場合, 書き込みをしない
	if (any(currentId >= size)) {
		return;
	}

	float4 input  = gInput[currentId];
	float depth   = FixedDepth(gDepth.Load(uint3(currentId, 0)));
	
	float4 output = (float4)0;

	float fixedForcusLegth = max(gParam.focusLength, 0.0f);

	// ピントからの相対距離を計算
	float coc = abs(depth - fixedForcusLegth) / depth;

	// 絞りに基づくぼかし半径を計算
	float blurRadius = coc * (fixedForcusLegth / gParam.f);
	float distanceFactor = saturate(abs(depth - fixedForcusLegth) / fixedForcusLegth);
	blurRadius *= lerp(1.0, 24.0f, pow(distanceFactor, 2.0)); // 距離に応じてぼかしを増幅

	float4 color = (float4)0;
	float weightSum = 0;

	const int2 diff = int2(8, 8);

	for (int x = -diff.x; x <= diff.x; ++x) {
		for (int y = -diff.y; y <= diff.y; ++y) {

			float weight = Gaussian2d(float2(x, y), blurRadius);
			color += weight * Sample(currentId, int2(x, y));
			weightSum += weight;

		}
	}

	color = saturate(color / weightSum);
	
	if (gParam.isDebugDoF) {
		gOutput[currentId] = float4(saturate(blurRadius), saturate(blurRadius), saturate(blurRadius), input.a);
		return;
	}
	
	gOutput[currentId] = color;

}
