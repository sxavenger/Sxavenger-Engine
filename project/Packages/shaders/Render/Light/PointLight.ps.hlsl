//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* component
#include "../../Component/PointLightComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<PointLightComponent> gParameter : register(b0);
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
	float3 l       = normalize(p_light - surface.position);      //!< lightの方向ベクトル
	float r        = length(p_light - surface.position);         //!< lightとsurfaceの距離

	float3 c_light = gParameter.GetColor(r);

	// 影の計算
	RayDesc ray;
	ray.Origin    = surface.position;
	ray.Direction = l;
	ray.TMin      = kTMin;
	ray.TMax      = r;
	
	c_light *= gShadow.TraceShadow(ray);

	//* Cameraの情報を取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);

	//* 計算
	float3 diffuse  = CalculateDiffuseLambert(surface.normal, l) * surface.albedo * c_light;
	float3 specular = CalculateSpecularBlinnPhong(surface.normal, l, v) * c_light;

	output.color.rgb = diffuse;

	output.color.a = 1.0f;
	return output;
	
}


