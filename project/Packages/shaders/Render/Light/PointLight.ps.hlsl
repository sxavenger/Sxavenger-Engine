//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct PointLight {
	float4 color_intensity; //!< rgb : color, a : intensity
	float distance;
};
ConstantBuffer<PointLight> gPointLight : register(b0);

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
	
	float3 c_light = gPointLight.color_intensity.rgb * gPointLight.color_intensity.a * dist;
	
	RayQuery<0> q;
	
	RayDesc ray;
	ray.Origin    = surface.position;
	ray.Direction = l;
	ray.TMin      = 0.001f;
	ray.TMax      = r;
	
	q.TraceRayInline(
		gScene,
		RAY_FLAG_NONE,
		0xFF,
		ray
	);
	
	if (q.Proceed()) { //!< 衝突した場合
		c_light /= 2.0f;

	}
	
	//* 出力
	output.color.rgb = diffuse * c_light * surface.albedo;
	// func_unlit() = float3(0.0f, 0.0f, 0.0f), func_lit() = c_surface
	
	output.color.a = 1.0f;
	
	return output;
	
}
