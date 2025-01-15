//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct PointLight {
	float4 color_intensity; //!< rgb : color, a : intensity
	float radius;
	float decay;
};
ConstantBuffer<PointLight> gPointLight : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {
	
	PSOutput output = (PSOutput)0;

	//* Deferred Pass情報の取得
	float2 uv = input.position.xy / gInfo.size;
	
	float3 worldPos = GetPosition(uv);
	float3 normal   = GetNormal(uv);
	
	//* Lightの情報を取得
	float3 center = gTransform[input.instanceId].GetPosition(); //!< Lightの中心座標

	//* Light輝度計算
	float3 l = normalize(center - worldPos); //!< Lightの方向ベクトル
	float d  = length(center - worldPos);    //!< Lightの距離
	
	float factor = saturate(1.0 - d / gPointLight.radius);
	
	//* 出力情報の書き込み
	output.color.rgb = gPointLight.color_intensity.rgb;
	output.color.a   = gPointLight.color_intensity.a * factor;
	
	if (output.color.a >= 0.0f) {
		discard;
	}
	
	return output;
}
