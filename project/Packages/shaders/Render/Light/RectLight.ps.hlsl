//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* component
// todo: RectLightComponent.hlsliに移動
#include "../../Library/Photometry.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

// todo: RectLightComponent.hlsliに移動
struct RectLightComponent {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float3 color;
	LightUnits::Type unit;
	float intensity;
	float3 size;
	float range;

	//=========================================================================================
	// public methods
	//=========================================================================================

	bool IsInRect(float3 position) {
		const float3 min = gTransforms[0].GetPosition() - size * 0.5f;
		const float3 max = gTransforms[0].GetPosition() + size * 0.5f;

		return all(position >= min) && all(position <= max);
	}

	bool IsInRangeRect(float3 position) {
		const float3 min = gTransforms[0].GetPosition() - (size + range) * 0.5f;
		const float3 max = gTransforms[0].GetPosition() + (size + range) * 0.5f;

		return all(position >= min) && all(position <= max);
	}

	float3 GetRectangle(float3 position) {
		const float3 min = -size * 0.5f + gTransforms[0].GetPosition();
		const float3 max = size * 0.5f + gTransforms[0].GetPosition();

		return clamp(position, min, max); //!< fixed position on the rectangle
	}

	float GetDistance(float3 position) {
		float3 rectangle = GetRectangle(position);
		return length(rectangle - position);
	}

	float3 GetDirection(float3 position) {
		float3 rectangle = GetRectangle(position);
		return normalize(rectangle - position);
	}

	float GetRadiance(float3 position) {
		float radiance = 0.0f;

		// todo: unitの変換を考慮する
		//switch (unit) {
		//	case LightUnits::Lumen:
		//		radiance = intensity / (4.0f * kPi);
		//		break;

		//	case LightUnits::Candela:
		//		radiance = intensity;
		//		break;
		//};
		radiance = intensity;

		if (IsInRect(position)) {
			return radiance; //!< not attenuated
		}

		if (!IsInRangeRect(position)) {
			return 0.0f; //!< out of range
		}

		float r = GetDistance(position);

		return 0.5f;
	}

	float3 GetColor(float3 position) {
		return color * GetRadiance(position);
	}
	
};
ConstantBuffer<RectLightComponent> gParameter : register(b0);
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
	float3 l = gParameter.IsInRect(surface.position)
		? surface.normal : gParameter.GetDirection(surface.position); //!< lightの方向ベクトル
	float r  = gParameter.GetDistance(surface.position);  //!< lightとsurfaceの距離

	float3 c_light = gParameter.GetColor(surface.position);

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
	float3 h = normalize(l + v);

	float NdotV = saturate(dot(surface.normal, v)) + kEpsilon; //!< 0除算対策
	float NdotL = saturate(dot(surface.normal, l)) + kEpsilon; //!< 0除算対策
	float NdotH = saturate(dot(surface.normal, h));
	float VdotH = saturate(dot(v, h));

	// f0
	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0

	// diffuse Albedo
	//!< 金属(metallic = 1.0f) -> 0.0f
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - f0)
	float3 diffuseAlbedo = surface.albedo * (1.0f - f0) * (1.0f - surface.metallic);

	// specular Albedo
	//!< 金属(metallic = 1.0f) -> f0
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 specularAlbedo = lerp(f0, surface.albedo, surface.metallic);

	float3 f  = F_SphericalGaussian(VdotH, specularAlbedo);
	float  vh = V_HeightCorrelated(NdotV, NdotL, surface.roughness);
	float  d  = D_GGX(NdotH, surface.roughness);

	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	float3 specularBRDF = SpecularBRDF(f, vh, d) * specularAlbedo;

	output.color.rgb = (diffuseBRDF + specularBRDF) * NdotL * c_light;

	output.color.a = 1.0f;
	return output;
	
}
