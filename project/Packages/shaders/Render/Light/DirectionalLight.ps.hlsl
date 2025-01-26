//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct DirectionalLight {
	float4 color_intencity; //!< rgb : color, a : intensity
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);
	
	//* Lightの情報を取得
	float3 l         = -gTransform[input.instanceId].GetDirection();                                //!< surfaceからlightへの方向ベクトル
	float3 c_light   = gDirectionalLight.color_intencity.rgb * gDirectionalLight.color_intencity.a; //!< lightのcolor
	
	//* 計算
	float diffuse = CalculateDiffuseHalfLambert(surface.normal, l);
	
	float3 v = normalize(gCamera.GetPosition() - surface.position);
	float speculer = CalculateSpecularBlinnPhong(surface.normal, l, v);
	
	//* 出力
	output.color.rgb = diffuse * c_light * surface.albedo + speculer * c_light;
	// func_unlit() = float3(0.0f, 0.0f, 0.0f), func_lit() = c_surface
	
	output.color.a = 1.0f;
	
	return output;
}
