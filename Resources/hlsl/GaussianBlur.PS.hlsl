//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "GaussianBlur.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// PSOutput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PSOutput {
	float4 color : SV_TARGET0;
};

//=========================================================================================
// TextureBuffers
//=========================================================================================
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

//=========================================================================================
// Buffers
//=========================================================================================
struct BlurParameter {
	float2 renderSize;      // renderTextureサイズ
	int isHolizontalEnable; // x軸方向へのブラーが有効か
	int isVerticlaEnable;   // y軸方向へのブラーが有効か
};
ConstantBuffer<BlurParameter> gParam : register(b0);

//-----------------------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------------------
static const float weight[3] = { 0.7f, 0.2f, 0.1 };

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////
float4 Sampling(float2 texcoord) {
	return gTexture.Sample(gSampler, texcoord);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {
	
	PSOutput output;
	output.color = 0.0f;
	
	for (uint i = 0; i < 3; ++i) {
		float2 offset = 0.0f;

		if (gParam.isHolizontalEnable) { // x軸のoffsetの生成
			offset.x = (i + 1.0f) / gParam.renderSize.x;
		}

		if (gParam.isVerticlaEnable) { // y軸のoffsetの生成
			offset.y = (i + 1.0f) / gParam.renderSize.y;
		}
		
		output.color += (weight[i] / 2.0f) * Sampling(input.texcoord + offset);
		output.color += (weight[i] / 2.0f) * Sampling(input.texcoord - offset);

	}
	
	output.color = saturate(output.color);
	
	return output;
}