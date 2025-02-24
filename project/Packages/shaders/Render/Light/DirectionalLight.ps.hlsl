//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct DirectionalLight {
	float3 color;
	float  intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b0);

ConstantBuffer<RayQueryShadow> gShadow : register(b1);

#define _BRDF

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _BRDF
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);

	//* Lightの情報を取得
	float3 l       = -gTransform[input.instanceId].GetDirection();          //!< surfaceからlightへの方向ベクトル
	float3 c_light = gDirectionalLight.color * gDirectionalLight.intensity; //!< lightのcolor

	//* BRDFの計算
	float3 v = normalize(gCamera.GetPosition() - surface.position);
	float3 h = normalize(l + v);

	float3 diffuse = dot(l, surface.normal);

	float specular = CalculateSpecularBRDF(surface.normal, v, l, surface.roughness);

	// vvv 仮 vvv //
	
	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = l;
	desc.TMin      = 0.001f;
	desc.TMax      = 10000.0f;

	c_light *= gShadow.TraceShadow(desc);

	output.color.rgb = diffuse * c_light * CalculateDiffuseBRDF(surface.albedo) + specular * CalculateDiffuseBRDF(c_light);
	//output.color.rgb = specular * CalculateDiffuseBRDF(c_light);

	output.color.a = 1.0f;
	return output;
}
#else
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);
	
	//* Lightの情報を取得
	float3 l       = -gTransform[input.instanceId].GetDirection();          //!< surfaceからlightへの方向ベクトル
	float3 c_light = gDirectionalLight.color * gDirectionalLight.intensity; //!< lightのcolor
	
	//* 計算
	float diffuse = CalculateDiffuseHalfLambert(surface.normal, l);
	
	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = l;
	desc.TMin      = 0.001f;
	desc.TMax      = 10000.0f;

	c_light *= gShadow.TraceShadow(desc);
	
	//* 出力
	output.color.rgb = diffuse * c_light * surface.albedo;
	// func_unlit() = float3(0.0f, 0.0f, 0.0f), func_lit() = c_surface
	
	output.color.a = 1.0f;
	
	return output;
}
#endif
