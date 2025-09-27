//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"
#include "../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<uint> gMoment      : register(u0);
RWTexture2D<float4> gReservoir : register(u1);

Texture2D<uint> gReferenceMoment      : register(t1);
Texture2D<float4> gReferenceReservoir : register(t2);

Texture2D<float4> gVelocity : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;

	if (CheckOverTexture(index)) {
		return;
	}

	float2 velocity = -gVelocity.Load(uint3(index, 0)).xy;
	velocity   *= 0.5f;  //!< uvの範囲に統一
	velocity.y *= -1.0f; //!< y軸反転
	
	float2 uv       = float2(index) / size; // [0.0 ~ 1.0]

	float2 x           = uv + velocity;
	uint2 sample_index = uint2(x * size);

	float4 reservoir = float4(0, 0, 0, 0);
	uint   moment   = 0;

	
	if (all(sample_index >= 0) && all(sample_index < size)) {
		// pixelが範囲内の場合, 前の情報を格納
		reservoir = gReferenceReservoir.Load(uint3(sample_index, 0));
		moment    = gReferenceMoment.Load(uint3(sample_index, 0));
	}

	gReservoir[index] = reservoir;
	gMoment[index]    = moment;
	
}
