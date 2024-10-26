//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Sprite.hlsli"

//=========================================================================================
// Buffers
//=========================================================================================
struct BlurParameter {
	uint2 textureSize;
	uint2 strength;
	float sigma;
};
ConstantBuffer<BlurParameter> gParam : register(b0);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

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
	if (gParam.strength.x > 0) {
		for (uint x = 1; x <= gParam.strength.x; ++x) {
		
			float weight = Gaussian(float(x), gParam.sigma); //!< 重みの計算
			output.color += weight * gTexture.Sample(gSampler, input.texcoord + float2(float(x) / gParam.textureSize.x, 0.0f)); //!< 正方向のブラー
			output.color += weight * gTexture.Sample(gSampler, input.texcoord - float2(float(x) / gParam.textureSize.x, 0.0f)); //!< 負方向のブラー
		
			weightSum += weight * 2.0f; //!< 両方向へのブラーなので重みが二倍
		}
		
	} else {
		output.color += gTexture.Sample(gSampler, input.texcoord);
		weightSum += 1.0f;
	}
		
	
	// 縦軸ブラー
	if (gParam.strength.y > 0) {
		for (uint y = 0; y < gParam.strength.y; ++y) {
		
			float weight = Gaussian(float(y), gParam.sigma); //!< 重みの計算
			output.color += weight * gTexture.Sample(gSampler, input.texcoord + float2(0.0f, float(y) / gParam.textureSize.y)); //!< 正方向のブラー
			output.color += weight * gTexture.Sample(gSampler, input.texcoord - float2(0.0f, float(y) / gParam.textureSize.y)); //!< 負方向のブラー
		
			weightSum += weight * 2.0f; //!< 両方向へのブラーなので重みが二倍
		}
	} else {
		output.color += gTexture.Sample(gSampler, input.texcoord);
		weightSum += 1.0f;
	}
		
	
	output.color /= weightSum; //!< 正規化
	
	return output;
	
}
