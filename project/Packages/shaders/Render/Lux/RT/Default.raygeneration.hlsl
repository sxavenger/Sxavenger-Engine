//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RayTracingCommon.hlsli"

//* library
#include "../../../Library/RandomLib.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
RAYGENERATION void mainRaygeneration() {
	//!< Downscale解像度Index想定.

	uint2 pixel     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	Lux::Setting setting = gSetting;

	Lux::Moment moment = Lux::Moment::GetMoment(gMoment[pixel]);

	if (moment.jitter == 0) {
		moment.jitter = Xorshift::xorshift32(pixel.x * pixel.y) + 1; //!< jitterの初期化. (pixel positionに基づく一意な値を設定)
	}

	GBuffer::Surface surface;
	if (!surface.FetchSurface(GBuffer::FetchArgument::Create(pixel * setting.downscale, resolution, gCamera.projInv, gCamera.world))) {
		//!< surfaceが存在しない場合
		return;
	}

	for (uint i = 0; i < setting.sampleStepCount; ++i) { //!< BRDF関与Rayの生成. (Diffuseのみなのでcos-weighted hemisphere samplingで設定)
		float2 xi        = Hammersley(moment.GetAtlasIndex(setting, i), setting.atlasDimension.x * setting.atlasDimension.y);
		float3 direction = ImportanceSampleCosineWeight(xi, surface.normal);
		float pdf        = ImportanceSampleCosineWeightPDF(direction, surface.normal);

		Payload payload = Payload::TracePrimaryRay(surface.position, direction);

		uint2 radiance_pixel = pixel * setting.atlasDimension + moment.GetAtlasPixel(setting, i); //!< 全Atlas上のPixel座標
		gBRDFRadianceCache[radiance_pixel] = float4(payload.lo, pdf);
	}
	

	// TODO: Light関与Rayの生成. (Lightのサンプリング方法に応じて設定)

	//!< Momentの更新.
	moment.Update(setting);
	gMoment[pixel] = moment.GetValue();
}

