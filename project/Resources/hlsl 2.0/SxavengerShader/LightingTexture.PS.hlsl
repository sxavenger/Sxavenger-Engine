//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Common
#include "SxavengerCommon.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================
ConstantBuffer<ObjectMaterial> gMaterial : register(b0);
StructuredBuffer<Light> gLight : register(t0);

Texture2D<float4> gAlbed : register(t1);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {
	
	PSOutput output;
	
	// Textureから色の取得
	float4 albed = gMaterial.GetColor(input.texcoord, gAlbed, gSampler) * gMaterial.color;
	CheckDiscard(albed);
	
	// lighting処理
	for (int i = 0; i < kMaxLightNum; ++i) {
		
		if (gLight[i].type == LightType::LIGHT_NONE) {
			continue; //!< ライトなし設定なのでスキップ
		}
		
		if (gLight[i].color.a == 0.0f) {
			continue; //!< 明るさ0なのでスキップ
		}
		
		if (gMaterial.lambertType == LamertType::TYPE_LAMBERT) {
			albed = gLight[i].GetLambertColor(input.normal, input.worldPos, albed);
			
		} else if (gMaterial.lambertType == LamertType::TYPE_HALF_LAMBERT) {
			albed = gLight[i].GetHalfLambertColor(input.normal, input.worldPos, albed);
		}
		
		//if () {
		
		//} else if () {
		
		//}
		
	}
	
	output.color = albed;
	
	CheckDiscard(output.color);
	return output;
	
}