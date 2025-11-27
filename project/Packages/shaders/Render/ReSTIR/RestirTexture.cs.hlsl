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
	
	float3 color = r.sample.lo * r.w;

	gIndirect[pixel] = float4(color, 1.0f);
	
}
