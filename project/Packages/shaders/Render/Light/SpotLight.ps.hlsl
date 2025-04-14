//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct SpotLight {
	float3 color;
	float intensity;
	float distance;
	float falloff; //!< theta_p
	float angle;   //!< theta_u
};
ConstantBuffer<SpotLight> gSpotLight : register(b0);

ConstantBuffer<InlineShadow> gShadow : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);
	
	//* Lightの情報を取得
	float3 p_light = gTransforms[input.instanceId].GetPosition(); //!< lightの中心座標
	
	//* 計算
	//!< func_diffuse(n, l)
	float3 l      = normalize(p_light - surface.position); //!< lightの方向ベクトル
	float diffuse = CalculateDiffuseHalfLambert(surface.normal, l);
	
	//!< func_dist(r) = func_win(r);
	float r    = length(p_light - surface.position); //!< lightとsurfaceの距離
	float dist = pow(max(1.0f - pow(r / gSpotLight.distance, 4.0f), 0.0f), 2.0f); //!< dist = func_win(r);
	
	//!< func_dir(l)
	float theta_s = dot(-l, gTransforms[input.instanceId].GetDirection());
	float t       = saturate((theta_s - gSpotLight.angle) / (gSpotLight.falloff - gSpotLight.angle));
	//float dir     = t * t;
	float dir     = t * t * (3.0f - 2.0f * t); //!< smoothstep

	float3 c_light = gSpotLight.color * gSpotLight.intensity * dist * dir;
	
	RayDesc ray;
	ray.Origin    = surface.position;
	ray.Direction = l;
	ray.TMin      = kTMin;
	ray.TMax      = r;

	c_light *= gShadow.TraceShadow(ray);
	
	//* 出力
	output.color.rgb = diffuse * c_light * surface.albedo;
	// func_unlit() = float3(0.0f, 0.0f, 0.0f), func_lit() = c_surface
	
	output.color.a = 1.0f;
	
	return output;
	
}

