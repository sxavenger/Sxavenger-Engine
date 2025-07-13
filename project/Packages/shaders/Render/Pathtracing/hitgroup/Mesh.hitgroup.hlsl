//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* hitgroup
#include "HitgroupCommon.hlsli"
#include "ImportanceSample.hlsli"

//* content
#include "../../../Content/Material.hlsli"
#include "../../../Library/Hammersley.hlsli"
#include "../../../Library/BRDF.hlsli"

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

float3 BSDF(float3 albedo, float roughness, float metallic, float3 normal, float3 wi, float3 wo) {

	float3 h = normalize(wi + wo);

	float NdotV = saturate(dot(normal, wi));
	float NdotL = saturate(dot(normal, wo));
	float NdotH = saturate(dot(normal, h));
	float VdotH = saturate(dot(wi, h));

	// diffuse Albedo
	//!< 金属(metallic = 1.0f) -> 0.0f
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - f0)
	//float3 diffuseAlbedo = surface.albedo * (1.0f - f0) * (1.0f - surface.metallic);
	float3 diffuseAlbedo = albedo * (1.0f - float3(0.04f, 0.04f, 0.04f)) * (1.0f - metallic);

	// specular Albedo
	//!< 金属(metallic = 1.0f) -> default-f0
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 f0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);

	float3 f = F_SphericalGaussian(VdotH, f0);
	float vh = V_HeightCorrelated(NdotV, NdotL, roughness);
	float d  = D_GGX(NdotH, roughness);

	//float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	float3 specularBRDF = SpecularBRDF(f, vh, d);

	return (/*diffuseBRDF + */specularBRDF) * NdotL;
	
}

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

	if (payload.IsPrimary()) {
		// primaryの場合の処理
		payload.SetPrimaryParameter(
			surface.position, 
			surface.normal, 
			0.0f, //!< ambient occlusionは未実装
			surface.roughness, 
			surface.metallic
		);
	}

	//!< hit処理
	payload.color.a = 1.0f;

	if (!gReservoir.CheckNeedSample()) {
		return; //!< sample数が足りている場合は何もしない.
	}

	//!< 乱数サンプルを生成
	float3 color = float3(0.0f, 0.0f, 0.0f);
	for (uint i = 0; i < gReservoir.frameSampleCount; ++i) {
		
		uint currentSampleNum = gReservoir.currentFrame * gReservoir.frameSampleCount + i;
		uint2 xi = Hammersley(currentSampleNum, gReservoir.sampleCount);

		{ // 拡散BRDFのサンプリング
			float3 dir = ImportanceSampleLambert(xi, surface.normal);

			RayDesc desc;
			desc.Origin = surface.position;
			desc.Direction = dir;
			desc.TMin = kTMin;
			desc.TMax = kTMax;

			Payload path = (Payload)0;
			path.rayType = RayType::kPath;

			float3 brdf = surface.albedo / kPi;
			float pdf = saturate(dot(surface.normal, dir)) / kPi;

			path.le = payload.le * brdf / pdf;
		
			payload.TraceRecursionRay(path, desc, kFlag);
			color += path.color.rgb;
		}

		{ // 鏡面BRDFのサンプリング
			float3 dir = ImportanceSampleGGX(xi, surface.roughness, surface.normal);

			RayDesc desc;
			desc.Origin    = surface.position;
			desc.Direction = dir;
			desc.TMin      = kTMin;
			desc.TMax      = kTMax;
			
			Payload path = (Payload)0;
			path.rayType = RayType::kPath;
			
			float3 brdf = BSDF(surface.albedo, surface.roughness, surface.metallic, surface.normal, -desc.Direction, -WorldRayDirection());
			float pdf   = saturate(dot(surface.normal, dir)) * D_GGX(saturate(dot(surface.normal, dir)), surface.roughness) / kPi;
			
			path.le = payload.le * brdf / pdf;
			payload.TraceRecursionRay(path, desc, kFlag);
			color += path.color.rgb;
			
		}
	}

	color /= float(gReservoir.sampleCount); //!< 平均化
	payload.color.rgb += color;
	
}
