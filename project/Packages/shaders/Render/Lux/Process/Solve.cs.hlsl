//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* Lux
#include "ProcessCommon.hlsli"

//* component
#include "../../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

RWTexture2D<float4> gIndirect : register(u0);

//* probe
Texture2D<float4> gIrradiance : register(t0);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	//!< InidrectのDispatch想定

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	GBuffer::Surface reference_surface; //!< pixelのsurface
	if (!reference_surface.FetchSurface(GBuffer::FetchArgument::Create(pixel, dimension, gCamera.projInv, gCamera.world))) {
		return; //!< surfaceが存在しない場合
	}

	uint2 irradiance_dimension = (dimension / setting.downscale) * setting.atlasDimension; //!< irradianceのテクスチャサイズ

	uint2 downscale_rect[4];
	downscale_rect[0] = pixel / setting.downscale * setting.downscale;     //!< downscale後の左上のピクセル座標
	downscale_rect[1] = downscale_rect[0] + uint2(setting.downscale.x, 0); //!< downscale後の右上のピクセル座標
	downscale_rect[2] = downscale_rect[0] + uint2(0, setting.downscale.y); //!< downscale後の左下のピクセル座標
	downscale_rect[3] = downscale_rect[0] + setting.downscale;             //!< downscale後の右下のピクセル座標

	float2 local = (float2(pixel) - float2(downscale_rect[0])) / float2(setting.downscale);

	float weight[4];
	weight[0] = (1.0f - local.x) * (1.0f - local.y);
	weight[1] = local.x * (1.0f - local.y);
	weight[2] = (1.0f - local.x) * local.y;
	weight[3] = local.x * local.y;
	
	uint2 atlas_rect[4]; 
	atlas_rect[0] = downscale_rect[0] / setting.downscale * setting.atlasDimension; //!< atlasの左上のピクセル座標 fixme?
	atlas_rect[1] = atlas_rect[0] + uint2(setting.atlasDimension.x, 0); //!< atlasの右上のピクセル座標
	atlas_rect[2] = atlas_rect[0] + uint2(0, setting.atlasDimension.y); //!< atlasの左下のピクセル座標
	atlas_rect[3] = atlas_rect[0] + setting.atlasDimension;             //!< atlasの右下のピクセル座標

	float3 irradiance[4];
	uint count = 0;
	for (uint i = 0; i < 4; ++i) {
		if (any(atlas_rect[i] >= irradiance_dimension)) {
			irradiance[i] = float3(0.0f, 0.0f, 0.0f);
			continue;
		}

		GBuffer::Surface sample_surface; //!< atlas内のsurface
		if (!sample_surface.FetchSurface(GBuffer::FetchArgument::Create(downscale_rect[i], dimension, gCamera.projInv, gCamera.world))) {
			irradiance[i] = float3(0.0f, 0.0f, 0.0f);
			continue; //!< surfaceが存在しない場合
		}

		if (dot(reference_surface.normal, sample_surface.normal) <= 0.0f) {
			irradiance[i] = float3(0.0f, 0.0f, 0.0f);
			continue; //!< reference_surfaceとsample_surfaceの法線が向いている方向が異なる場合
		}

		float3 direction = Lux::ConvertDirection(reference_surface.normal, sample_surface.normal); //!< sample_surface.normalを基準にreference_surface.normalを変換した方向
		float2 uv = saturate(Octahedral::EncodePyramid(direction)); //!< 方向からuv座標を算出

		uint2 sample_pixel = atlas_rect[i] + uint2(uv * setting.atlasDimension); //!< atlas内のサンプルするピクセル座標
		irradiance[i] = gIrradiance.Load(uint3(sample_pixel, 0)).rgb; //!< サンプルしたirradiance
	}

	float3 lo = float3(0.0f, 0.0f, 0.0f);
	for (uint i = 0; i < 4; ++i) {
		lo += irradiance[i] * weight[i];
	}

	BxDFAlbedo albedo = BxDFAlbedo::Create(reference_surface.albedo, reference_surface.metallic);

	float3 color = albedo.diffuse * lo * reference_surface.ao; //!< 簡易的にdiffuse成分のみを考慮
	
	gIndirect[pixel] = float4(lo, 1.0f); //!< 4つのサンプルのうち1つ目を格納. 実際には4つのサンプルを利用して補間するなどする想定.]
}
