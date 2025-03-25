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
	float3 l       = -gTransform[input.instanceId].GetDirection(); //!< surfaceからlightへの方向ベクトル
	float3 c_light = gParameter.color * gParameter.intensity;      //!< lightのcolor

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);

	//* shadow
	RayDesc desc;
	desc.Origin    = surface.position;
	desc.Direction = l;
	desc.TMin      = 0.001f;
	desc.TMax      = 10000.0f;

	c_light *= gShadow.TraceShadow(desc);
	// todo: 不必要な場合は、gShadow.TraceShadow()を呼び出さないようにする

	// 表面化albedo = a_surface

	// f0
	// m = 0 -> f0 = 0.0, m = 1 -> f0 = a_surface = c_surface 
	float3 f0 = lerp(surface.albedo, float3(0.0f, 0.0f, 0.0f), surface.metallic);

	// f(l, v) = a_surface / kPi
	float3 diffuse = f0 / kPi;

	float3 spec = CalculateSpecularBRDF(surface.normal, v, l, surface.roughness, f0);

	//* 出力

	// ローカル照明での計算
	// L_i(l) = l
	// L_o(v) = f(l, v) * c_light * saturate(dot(n, l))
	output.color.rgb = diffuse * c_light * saturate(dot(surface.normal, l)) + spec;

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
