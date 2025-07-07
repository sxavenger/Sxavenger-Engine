//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* hitgroup
#include "HitgroupCommon.hlsli"
#include "ImportanceSample.hlsli"

//* content
#include "../../../Content/Material.hlsli"
#include "../../../Library/Hammersley.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

StructuredBuffer<Material> gMaterial : register(t0);
//!< FIXME: ConstantBufferで使えるようにする. 多分alignment問題
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// Surface structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Surface {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float3 position;
	float3 normal;
	float3 albedo;
	float roughness;
	float metallic;

	//=========================================================================================
	// public methods
	//=========================================================================================

	void GetSurface(Attribute attribute) {
		
		MeshVertex vertex = GetWorldVertex(attribute);

		MaterialLib::TextureSampler parameter;
		parameter.Set(vertex.texcoord, gSampler);

		position = vertex.position.xyz;

		float3x3 tbn = float3x3(
			vertex.tangent, 
			vertex.bitangent, 
			vertex.normal
		);
		normal = gMaterial[0].normal.GetNormal(vertex.normal, parameter, tbn);

		albedo    = gMaterial[0].albedo.GetAlbedo(parameter);
		roughness = gMaterial[0].properties.roughness.GetValue(parameter, 1);
		metallic  = gMaterial[0].properties.metallic.GetValue(parameter, 2);
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 CalculateDirectionalLight(uint index, float3 position, float3 normal, float3 albedo, float roughness, float metallic) {
	//* Lightの情報を取得
	float3 c_light = gDirectionalLights[index].GetColor(); //!< lightのcolor
	float3 l       = -gDirectionalLightTransforms[index].GetDirection(); //!< surfaceからlightへの方向ベクトル

	//* shadow
	RayDesc desc;
	desc.Origin    = position;
	desc.Direction = l;
	desc.TMin      = kTMin;
	desc.TMax      = kTMax;

	c_light *= gDirectionalLightShadows[index].TraceShadow(desc, gScene);
	// todo: 不必要な場合は、gShadow.TraceShadow()を呼び出さないようにする

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - position);

	//* 計算
	float3 h = normalize(l + v);

	float NdotV = saturate(dot(normal, v));
	float NdotL = saturate(dot(normal, l));
	float NdotH = saturate(dot(normal, h));
	float VdotH = saturate(dot(v, h));

	// f0
	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0

	// diffuse Albedo
	//!< 金属(metallic = 1.0f) -> 0.0f
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - f0)
	//float3 diffuseAlbedo = surface.albedo * (1.0f - f0) * (1.0f - surface.metallic);
	float3 diffuseAlbedo = albedo * (1.0f - f0) * (1.0f - metallic);

	// specular Albedo
	//!< 金属(metallic = 1.0f) -> f0
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 specularAlbedo = lerp(f0, albedo, metallic);

	float3 f = F_SphericalGaussian(VdotH, specularAlbedo);
	float vh = V_HeightCorrelated(NdotV, NdotL, roughness);
	float d  = D_GGX(NdotH, roughness);

	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	float3 specularBRDF = SpecularBRDF(f, vh, d);

	return (diffuseBRDF + specularBRDF) * NdotL * c_light;
}

float3 CalculatePointLight(uint index, float3 position, float3 normal, float3 albedo, float roughness, float metallic) {

	//* Lightの情報を取得
	float3 p_light = gPointLightTransforms[index].GetPosition(); //!< lightの中心座標
	float3 l       = normalize(p_light - position);              //!< lightの方向ベクトル
	float r        = length(p_light - position);                 //!< lightとsurfaceの距離

	float3 c_light = gPointLights[index].GetColor(r);

	// 影の計算
	RayDesc desc;
	desc.Origin    = position;
	desc.Direction = l;
	desc.TMin      = kTMin;
	desc.TMax      = r;
	
	c_light *= gPointLightShadows[index].TraceShadow(desc, gScene);

	//* Cameraの情報を取得
	float3 v = normalize(gCamera.GetPosition() - position);

	//* 計算
	float3 h = normalize(l + v);

	float NdotV = saturate(dot(normal, v)); //!< 0除算対策
	float NdotL = saturate(dot(normal, l)); //!< 0除算対策
	float NdotH = saturate(dot(normal, h));
	float VdotH = saturate(dot(v, h));

	// f0
	static const float3 f0 = float3(0.04f, 0.04f, 0.04f); //!< 非金属の場合のf0

	// diffuse Albedo
	//!< 金属(metallic = 1.0f) -> 0.0f
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - f0)
	float3 diffuseAlbedo = albedo * (1.0f - f0) * (1.0f - metallic);

	// specular Albedo
	//!< 金属(metallic = 1.0f) -> f0
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 specularAlbedo = lerp(f0, albedo, metallic);

	float3 f  = F_SphericalGaussian(VdotH, specularAlbedo);
	float  vh = V_HeightCorrelated(NdotV, NdotL, roughness);
	float  d  = D_GGX(NdotH, roughness);

	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	float3 specularBRDF = SpecularBRDF(f, vh, d);

	return (diffuseBRDF + specularBRDF) * NdotL * c_light;
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// anyhit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_ANYHIT void mainAnyhit(inout Payload payload, in Attribute attribute) {

	MeshVertex vertex = GetWorldVertex(attribute);

	MaterialLib::TextureSampler parameter;
	parameter.Set(vertex.texcoord, gSampler);
	
	float transparency = gMaterial[0].transparency.GetTransparency(parameter);
	if (transparency <= 0.1f) {
		IgnoreHit(); //!< 透明度が低い場合は、ヒットを無視する
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// closesthit main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_CLOSESTHIT void mainClosesthit(inout Payload payload, in Attribute attribute) {

	Surface surface;
	surface.GetSurface(attribute);

	if (payload.IsPrimary() && gReservoir.currentFrame == 0) { //!< primary初回hit時
		// parameterの設定
		payload.SetPrimaryParameter(surface.position, surface.normal, 0.0f, surface.roughness, surface.metallic);

		// PrimarySurfaceのLighting計算
		payload.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
		
		for (uint i = 0; i < gDirectionalLightCount.count; ++i) {
			payload.color.rgb += CalculateDirectionalLight(i, surface.position, surface.normal, surface.albedo, surface.roughness, surface.metallic);
		}

		for (uint i = 0; i < gPointLightCount.count; ++i) {
			payload.color.rgb += CalculatePointLight(i, surface.position, surface.normal, surface.albedo, surface.roughness, surface.metallic);
		}
	}

	// 反射レイの処理
	if (payload.rayType == RayType::kReflection) {
		payload.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
		
		for (uint i = 0; i < gDirectionalLightCount.count; ++i) {
			payload.color.rgb += CalculateDirectionalLight(i, surface.position, surface.normal, surface.albedo, surface.roughness, surface.metallic);
		}

		for (uint i = 0; i < gPointLightCount.count; ++i) {
			payload.color.rgb += CalculatePointLight(i, surface.position, surface.normal, surface.albedo, surface.roughness, surface.metallic);
		}
	}

	if (gReservoir.CheckNeedSample() && payload.CheckNextRecursion()) {
		// パストレース
		float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
		for (uint i = 0; i < gReservoir.frameSampleCount; ++i) {
			uint currentSampleNum = gReservoir.currentFrame * gReservoir.frameSampleCount + i;
			uint2 xi = Hammersley(currentSampleNum, gReservoir.sampleCount);

			float3 direction = ImportanceSampleLambert(xi, surface.normal);

			RayDesc desc;
			desc.Origin = surface.position;
			desc.Direction = direction;
			desc.TMin = kTMin;
			desc.TMax = kTMax;

			Payload path;
			path.Reset();
			path.rayType = RayType::kReflection;

			payload.TraceRecursionRay(path, desc);
			color += path.color;
		}

		color.rgb /= float(gReservoir.sampleCount);
		payload.color += color;
	}

	
}
