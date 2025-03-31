//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct Parameter {
	float3 color;
	float  intensity;
};
ConstantBuffer<Parameter> gParameter : register(b0);

ConstantBuffer<InlineShadow> gShadow : register(b1);

#define _USE_PBR

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _USE_PBR
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;

	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);

	//* Lightの情報を取得
	float3 c_light = gParameter.color * gParameter.intensity; //!< lightのcolor
	float3 l       = -gTransform[input.instanceId].GetDirection(); //!< surfaceからlightへの方向ベクトル

	//* shadow
	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = l;
	desc.TMin      = kTMin;
	desc.TMax      = kTMax;

	c_light *= gShadow.TraceShadow(desc);
	// todo: 不必要な場合は、gShadow.TraceShadow()を呼び出さないようにする

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);

	//* 計算

	// f0
	// 非金属(metalness = 0) -> の時は固定数
	// 金属(metalness = 1) -> の時はalbedoをf0

	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0

	// diffuse BRDF
	float3 diffuseAlbedo = surface.albedo * (float3(1.0f, 1.0f, 1.0f) - f0) * (1.0f - surface.metallic);
	float3 diffuseBRDF   = diffuseAlbedo / kPi;

	// specular BRDF
	float3 specularAlbedo = lerp(f0, surface.albedo, surface.metallic);
	float3 specularBRDF   = CalculateSpecularBRDF(surface.normal, v, l, surface.roughness, specularAlbedo);

	output.color.rgb = (diffuseBRDF + specularBRDF) * saturate(dot(surface.normal, l)) * c_light;

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
	float3 l       = -gTransform[input.instanceId].GetDirection(); //!< surfaceからlightへの方向ベクトル
	float3 c_light = gParameter.color * gParameter.intensity;      //!< lightのcolor

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);
	
	//* 計算
	float3 diffuse = CalculateDiffuseLambert(surface.normal, l) * surface.albedo;
	
	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = l;
	desc.TMin      = 0.001f;
	desc.TMax      = 10000.0f;

	c_light *= gShadow.TraceShadow(desc);
	// todo: 不必要な場合は、gShadow.TraceShadow()を呼び出さないようにする
	
	//* 出力
	output.color.rgb = diffuse * c_light;
	// func_unlit() = float3(0.0f, 0.0f, 0.0f), func_lit() = c_surface
	
	output.color.a = 1.0f;
	
	return output;
}
#endif
