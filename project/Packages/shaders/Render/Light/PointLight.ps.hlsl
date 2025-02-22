//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct PointLight {
	float3 color;
	float intensity;
	float distance;
};
ConstantBuffer<PointLight> gPointLight : register(b0);

ConstantBuffer<RayQueryShadow> gShadow : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);
	
	//* Lightの情報を取得
	float3 p_light = gTransform[input.instanceId].GetPosition(); //!< lightの中心座標
	
	//* 計算
	//!< func_diffuse(n, l)
	float3 l      = normalize(p_light - surface.position); //!< lightの方向ベクトル
	float diffuse = CalculateDiffuseHalfLambert(surface.normal, l);
	
	//!< func_dist(r) = func_win(r);
	float r    = length(p_light - surface.position); //!< lightとsurfaceの距離
	float dist = pow(max(1.0f - pow(r / gPointLight.distance, 4.0f), 0.0f), 2.0f); //!< dist = func_win(r);

	float3 v = normalize(gCamera.GetPosition() - surface.position);
	float3 phong = CalculateSpecularBlinnPhong(surface.normal, l, v);
	
	float3 c_light = gPointLight.color * gPointLight.intensity * dist;
	
	RayDesc ray;
	ray.Origin    = surface.position;
	ray.Direction = l;
	ray.TMin      = 0.001f;
	ray.TMax      = r;
	
	c_light *= gShadow.TraceShadow(ray);
	
	//* 出力
	output.color.rgb = diffuse * c_light * surface.albedo;
	// func_unlit() = float3(0.0f, 0.0f, 0.0f), func_lit() = c_surface
	
	output.color.a = 1.0f;
	
	return output;
	
}
