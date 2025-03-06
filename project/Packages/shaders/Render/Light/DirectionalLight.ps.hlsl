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

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;
	
	//* Deferred Pass情報の取得
	Surface surface;
	surface.GetSurface(input.position.xy);
	
	//* Lightの情報を取得
	float3 l       = -gTransform[input.instanceId].GetDirection();          //!< surfaceからlightへの方向ベクトル
	float3 c_light = gParameter.color * gParameter.intensity; //!< lightのcolor
	
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
