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
Texture2D<float4> gRadianceCache : register(t0);

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

	uint2 x = pixel % setting.downscale; //!< downscale後のpixel座標

	
	
}
