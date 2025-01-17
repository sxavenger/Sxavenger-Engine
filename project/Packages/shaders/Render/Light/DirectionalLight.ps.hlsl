//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

struct DirectionalLight {
	float4 color_intencity; //!< rgb : color, a : intensity
	float3 direction;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	// Objectが存在しない場合は何もしない
	if (GetDepth(input.position.xy) == 1.0f) {
		discard;
	}
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);
	
	//* Lightの情報を取得
	float3 l       = -gDirectionalLight.direction;                                                //!< surfaceからlightへの方向ベクトル
	float3 c_light = gDirectionalLight.color_intencity.rgb * gDirectionalLight.color_intencity.a; //!< lightのcolor
	
	//* 計算
	float diffuse = CalculateDiffuseLambert(surface.normal, l);
	
	//* 出力
	output.color.rgb = diffuse * c_light * surface.albedo;
	// func_unlit() = float3(0.0f, 0.0f, 0.0f), func_lit() = c_surface
	
	output.color.a = 1.0f;
	
	return output;
}
