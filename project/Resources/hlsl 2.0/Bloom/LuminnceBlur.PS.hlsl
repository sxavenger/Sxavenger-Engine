//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Sprite.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

//=========================================================================================
// parameter
//=========================================================================================
static const float pi_v = 3.14159265358979323846f; // pi

// variable
static const float2 kWindowSize = float2(1280.0f, 720.0f);
static const uint2 blurStrength = uint2(24, 12);
static const float sigma = 12.0f;

//=========================================================================================
// methods
//=========================================================================================
float Gaussian(float x, float y) {
	return exp(-(x * x) / (2.0f * y * y));
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {
	
	PSOutput output;
	
	output.color = 0.0f;
	float weightSum = 0.0f;
	
	// 横軸ブラー
	for (uint x = 0; x < blurStrength.x; ++x) {
		
		float weight = Gaussian(float(x), sigma); //!< 重みの計算
		output.color += weight * gTexture.Sample(gSampler, input.texcoord + float2(float(x + 1) / kWindowSize.x, 0.0f)); //!< 正方向のブラー
		output.color += weight * gTexture.Sample(gSampler, input.texcoord - float2(float(x + 1) / kWindowSize.x, 0.0f)); //!< 負方向のブラー
		
		weightSum += weight * 2.0f; //!< 両方向へのブラーなので重みが二倍
	}
	
	// 縦軸ブラー
	for (uint y = 0; y < blurStrength.y; ++y) {
		
		float weight = Gaussian(float(y), sigma); //!< 重みの計算
		output.color += weight * gTexture.Sample(gSampler, input.texcoord + float2(0.0f, float(y + 1) / kWindowSize.y)); //!< 正方向のブラー
		output.color += weight * gTexture.Sample(gSampler, input.texcoord - float2(0.0f, float(y + 1) / kWindowSize.y)); //!< 負方向のブラー
		
		weightSum += weight * 2.0f; //!< 両方向へのブラーなので重みが二倍
	}
	
	output.color /= weightSum; //!< 正規化
	
	return output;
}