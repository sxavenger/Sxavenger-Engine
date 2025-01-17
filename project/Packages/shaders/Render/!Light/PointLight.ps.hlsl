//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"
#include "../../Light.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct PointLight {
	float4 color_intensity; //!< rgb : color, a : intensity
	float radius;
};
ConstantBuffer<PointLight> gPointLight : register(b0);

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
	float3 p_surface = GetPosition(input.position.xy); //!< surfaceの座標
	float3 c_surface = GetAlbedo(input.position.xy);   //!< surfaceの色
	float3 n         = GetNormal(input.position.xy);   //!< surfaceの法線
	
	//* Lightの情報を取得
	float3 p_light = gTransform[input.instanceId].GetPosition(); //!< lightの中心座標
	
	float3 l = normalize(p_light - p_surface); //!< lightの方向ベクトル
	float r  = length(p_light - p_surface);    //!< lightとsurfaceの距離

	float diffuse = CalculateDiffuseLambert(n, l);
	
	float dist     = pow(max(1.0f - pow(r / gPointLight.radius, 4.0f), 0.0f), 2.0f); //!< dist = func_win(r);
	float3 c_light = gPointLight.color_intensity.rgb * gPointLight.color_intensity.a * dist;
	
	//* 出力
	output.color.rgb = diffuse * c_light * c_surface;
	output.color.a   = 1.0f;
	
	return output;
}
