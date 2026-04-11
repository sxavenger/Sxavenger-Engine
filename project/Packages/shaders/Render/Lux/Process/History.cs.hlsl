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

RWTexture2D<float4> gHistory : register(u0);

//* probe
Texture2D<float4> gRadianceCache : register(t0);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////



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
		gHistory[pixel] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; //!< surfaceが存在しない
	}

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
	atlas_rect[0] = downscale_rect[0] / setting.downscale * setting.atlasDimension; //!< atlasの左上のピクセル座標
	atlas_rect[1] = atlas_rect[0] + uint2(setting.atlasDimension.x, 0);             //!< atlasの右上のピクセル座標
	atlas_rect[2] = atlas_rect[0] + uint2(0, setting.atlasDimension.y);             //!< atlasの左下のピクセル座標
	atlas_rect[3] = atlas_rect[0] + setting.atlasDimension;                         //!< atlasの右下のピクセル座標

	float3 lo = float3(0.0f, 0.0f, 0.0f);

	const float solid_angle = Mathmatic::kTau / (setting.atlasDimension.x * setting.atlasDimension.y);
	
	for (uint i = 0; i < 4; ++i) {

		float3 irradiance = float3(0.0f, 0.0f, 0.0f);
		
		GBuffer::Surface sample_surface;
		if (!sample_surface.FetchSurface(GBuffer::FetchArgument::Create(downscale_rect[i], dimension, gCamera.projInv, gCamera.world))) {
			continue; //!< surfaceが存在しない場合
		}
	
		for (uint x = 0; x < setting.atlasDimension.x; ++x) {
			for (uint y = 0; y < setting.atlasDimension.y; ++y) {

				uint2 sample_pixel = atlas_rect[i] + uint2(x, y);           //!< atlas内のピクセル座標
				float2 sample_uv   = float2(x, y) / setting.atlasDimension; //!< atlas内のuv座標
				
				float3 radiance  = gRadianceCache.Load(uint3(sample_pixel, 0)).rgb;
				float3 direction = Octahedral::DecodePyramid(sample_uv);

				float NdotL = max(0.0f, dot(Lux::ConvertDirection(direction, sample_surface.normal), reference_surface.normal));

				if (NdotL <= 0.0f) {
					continue;
				}

				irradiance += radiance * NdotL * solid_angle;
			}
		}
		
		irradiance *= weight[i];

		lo += irradiance;

	}
	
	BxDFAlbedo albedo = BxDFAlbedo::Create(reference_surface.albedo, reference_surface.metallic);
	
	float3 current_indirect  = lo * Diffuse_Lambert(albedo) * reference_surface.ao;
	float3 previous_indirect = gHistory[pixel].rgb;


	static const float kHysteresisAlpha = 0.9f; //!< [0.85, 0.95]
	
	gHistory[pixel] = float4(lerp(current_indirect, previous_indirect, 1.0f - kHysteresisAlpha), 1.0f);
}
