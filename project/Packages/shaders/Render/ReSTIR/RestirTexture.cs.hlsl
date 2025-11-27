//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RestirPass.hlsli"

//* common
#include "../DeferredBufferIndex.hlsli"

//* library
#include "../../Library/BRDF.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

RWStructuredBuffer<Reservoir> gReservoir : register(u0);
RWTexture2D<float4>           gIndirect  : register(u1);

//* deferred index
ConstantBuffer<DeferredBufferIndexConstantBuffer> gDeferredBufferIndex : register(b0);

ConstantBuffer<CameraComponent> gCamera : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREAD_X, _NUM_THREAD_Y, _NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	uint p = Flatten(pixel);

	Reservoir r = gReservoir[p];

	DeferredSurface surface;
	if (!surface.GetSurface(gDeferredBufferIndex.Get(), pixel)) {
		gIndirect[pixel] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; //!< サーフェス情報が無い
	}

	//* Lightの情報を取得
	float3 l = normalize(r.sample.xs - surface.position); //!< lightからの方向ベクトルを取得

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position); //!< cameraからの方向ベクトルを取得

	//* 計算
	float3 h = normalize(l + v);

	float NdotV = saturate(dot(surface.normal, v));
	float NdotL = saturate(dot(surface.normal, l));
	float NdotH = saturate(dot(surface.normal, h));
	float VdotH = saturate(dot(v, h));

	static const float3 kMinFrenel = float3(0.04f, 0.04f, 0.04f); //!< 非金属の最小Frenel値

	// diffuse Albedo
	//!< 金属(metallic = 1.0f) -> 0.0f
	//!< 非金属(metallic = 0.0f) -> albedo * (1.0f - kMinFrenel)
	float3 diffuseAlbedo = surface.albedo * (1.0f - kMinFrenel) * (1.0f - surface.metallic);

	// specular Albedo
	//!< 金属(metallic = 1.0f) -> kMinFrenel
	//!< 非金属(metallic = 0.0f) -> albedo
	float3 specularAlbedo = lerp(kMinFrenel, surface.albedo, surface.metallic);

	float3 f = F_SphericalGaussian(VdotH, specularAlbedo);
	float vh = V_HeightCorrelated(NdotV, NdotL, surface.roughness);
	float d  = D_GGX(NdotH, surface.roughness);

	float3 diffuseBRDF  = DiffuseBRDF(diffuseAlbedo);
	float3 specularBRDF = SpecularBRDF(f, vh, d);
	
	float3 color = (diffuseBRDF + specularBRDF) * r.sample.lo * r.w;

	gIndirect[pixel] = float4(color, 1.0f);
	
}
