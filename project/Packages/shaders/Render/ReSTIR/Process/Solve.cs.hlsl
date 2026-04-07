//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ReSTIR
#include "ReSTIRProcess.hlsli"

//* component
#include "../../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffer
//=========================================================================================

//* reservoir
StructuredBuffer<ReSTIR::Reservoir> gReservoir : register(t0);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b0);

//* output
RWTexture2D<float4> gIndirect : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;
	
	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	GBuffer::Surface surface;
	if (!surface.FetchSurface(GBuffer::FetchArgument::Create(pixel, dimension, gCamera.projInv, gCamera.world))) {
		gIndirect[pixel] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; //!< surfaceが存在しない場合
	}

	uint p = ReSTIR::FlattenIndex(pixel, dimension);
	ReSTIR::Reservoir reservoir = gReservoir[p];

	float3 lo = reservoir.sample.lo * reservoir.w;

	gIndirect[pixel] = float4(lo, 1.0f);
	
}

