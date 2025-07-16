//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Process.hlsli"
#include "../../../Library/ACES.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gTexture : register(u0); //!< input and output texture

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

/*
#reference
 - https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
*/
float3 ACESFilmicToneCurve(float3 x) {
	x = max(x, 0.0f); //!< 負の数は無視する
	
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}
// todo: パラメータで調整できるようにする.

float3 RRTAndODTFit(float3 v) {
	float3 a = v * (v + 0.0245786f) - 0.000090537f;
	float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
	return a / b;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float3 input = gTexture[index].rgb;

	static const float3x3 ACESInputMat = {
		{0.59719, 0.35458, 0.04823},
		{0.07600, 0.90834, 0.01566},
		{0.02840, 0.13383, 0.83777}
	};

	static const float3x3 ACESOutputMat = {
		{1.60475, -0.53108, -0.07367},
		{-0.10208, 1.10813, -0.00605},
		{-0.00327, -0.07276, 1.07602}
	};

	float3 aces_cg = mul(ACESInputMat, input.rgb); //!< 色空間変換
	aces_cg = RRTAndODTFit(aces_cg);

	gTexture[index].rgb = saturate(mul(ACESOutputMat, aces_cg)); //!< 色空間変換後の色を出力
	

}
