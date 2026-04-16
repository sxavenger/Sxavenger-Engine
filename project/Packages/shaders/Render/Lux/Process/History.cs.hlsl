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

//* cache
Texture2D<float4> gBRDFRadianceCache : register(t0);
Texture2D<uint> gMoment              : register(t1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

void CalculateWeight(uint2 surface_pixel, out float weight[4]) {
	float2 local = (float2(surface_pixel) - float2(setting.GetDownscalePixel(surface_pixel) * setting.downscale)) / float2(setting.downscale);

	weight[0] = (1.0f - local.x) * (1.0f - local.y);
	weight[1] = local.x * (1.0f - local.y);
	weight[2] = (1.0f - local.x) * local.y;
	weight[3] = local.x * local.y;
}

float3 CalculateIrradiance(uint2 surface_pixel, GBuffer::Surface surface, uint2 downscaled_pixel) {

	float3 irradiance = float3(0.0f, 0.0f, 0.0f);

	GBuffer::Surface sample_surface;
	if (!sample_surface.FetchSurface(GBuffer::FetchArgument::Create(downscaled_pixel * setting.downscale, dimension, gCamera.projInv, gCamera.world))) {
		return float3(0.0f, 0.0f, 0.0f); //!< surfaceが存在しない場合
	}

	uint2 atlas_pixel = downscaled_pixel * setting.atlasDimension; //!< atlas内の左上のピクセル座標

	Lux::Moment moment = Lux::Moment::GetMoment(gMoment[downscaled_pixel]);

	for (uint x = 0; x < setting.atlasDimension.x; ++x) {
		for (uint y = 0; y < setting.atlasDimension.y; ++y) {

			float2 xi        = Hammersley(moment.GetAtlasIndex(setting, uint2(x, y)), setting.atlasDimension.x * setting.atlasDimension.y);
			float3 direction = ImportanceSampleCosineWeight(xi, sample_surface.normal);

			uint2 sample_pixel = atlas_pixel + uint2(x, y); //!< atlas内のピクセル座標

			float4 cache = gBRDFRadianceCache.Load(uint3(sample_pixel, 0));

			float3 radiance = cache.rgb;
			float pdf       = cache.a;

			float NdotL = max(0.0f, dot(direction, surface.normal));

			if (NdotL <= 0.0f) {
				continue;
			}

			if (pdf <= 0.0f) {
				continue;
			}

			irradiance += radiance * NdotL / pdf;
		}
	}

	irradiance /= (setting.atlasDimension.x * setting.atlasDimension.y);
	return irradiance;
}

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

	float weight[4];
	CalculateWeight(pixel, weight);

	static const uint2 offset[4] = { uint2(0, 0), uint2(1, 0), uint2(0, 1), uint2(1, 1) };
	

	float3 lo = float3(0.0f, 0.0f, 0.0f);
	
	for (uint i = 0; i < 4; ++i) {
		float3 irradiance = CalculateIrradiance(pixel, reference_surface, setting.GetDownscalePixel(pixel) + offset[i]);
		lo += irradiance * weight[i];
	}
	
	BxDFAlbedo albedo = BxDFAlbedo::Create(reference_surface.albedo, reference_surface.metallic);
	
	float3 current_indirect  = lo * Diffuse_Lambert(albedo) * reference_surface.ao;
	float3 previous_indirect = gHistory[pixel].rgb;

	static const float kHysteresisAlpha = 1.0f; //!< [0.85, 0.95]
	
	gHistory[pixel] = float4(lerp(current_indirect, previous_indirect, 1.0f - kHysteresisAlpha), 1.0f);
}
