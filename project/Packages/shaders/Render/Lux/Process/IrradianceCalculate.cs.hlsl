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

RWTexture2D<float4> gIrradiance : register(u0);
Texture2D<float4> gRadianceCache : register(t0);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	//!< IrradianceのDispatchの想定.

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	GBuffer::Surface surface;
	if (!surface.FetchSurface(GBuffer::FetchArgument::Create(Lux::GetSurfacePixel(pixel, setting), dimension, gCamera.projInv, gCamera.world))) {
		gIrradiance[pixel] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; //!< surfaceが存在しない場合
	}

	float2 reference_uv        = float2(pixel % setting.atlasDimension) / setting.atlasDimension; //!< atlas内のuv座標から算出した基準方向
	float3 reference_direction = Octahedral::DecodePyramid(reference_uv); //!< 基準方向

	uint2 lefttop = pixel - (pixel % setting.atlasDimension); //!< atlasの左下のピクセル座標

	float3 irradiance = float3(0.0f, 0.0f, 0.0f);
	uint count = 0;

	//!< O(x * y)
	for (uint x = 0; x < setting.atlasDimension.x; ++x) {
		for (uint y = 0; y < setting.atlasDimension.y; ++y) {

			uint2 sample_pixel      = lefttop + uint2(x, y); //!< atlas内のピクセル座標
			float2 sample_uv        = float2(x, y) / setting.atlasDimension; //!< atlas内のuv座標
			float3 sample_direction = Octahedral::DecodePyramid(sample_uv);

			float3 radiance = gRadianceCache[sample_pixel].rgb;
			
			float NdotL = max(0.0f, dot(sample_direction, reference_direction));
			//!< surface.normalをどちらともlocalにしているため変換なし.

			if (NdotL > 0.0f) {
				irradiance += radiance * NdotL;
				count++;
			}
			
		}
	}

	if (count > 0) {
		irradiance /= count;
		irradiance *= Mathmatic::kPi;
	}
	
	gIrradiance[pixel] = float4(irradiance, 1.0f);
}
