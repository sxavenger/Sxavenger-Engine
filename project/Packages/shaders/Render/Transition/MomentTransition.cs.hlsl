//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"
#include "../../Component/CameraComponent.hlsli"
#include "../../Library/Random.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<uint2> gMoment     : register(u0);
RWTexture2D<float4> gReservoir : register(u1);

Texture2D<uint2> gReferenceMoment     : register(t1);
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

	float3 velocity = -gVelocity.Load(uint3(index, 0)).xyz;
	velocity   *= 0.5f; //!< uvの範囲に統一
	
	float2 uv       = float2(index) / size; // [0.0 ~ 1.0]

	float2 x           = uv + velocity.xy;
	uint2 sample_index = uint2(x * size);

	float4 reservoir = float4(0, 0, 0, 0);
	uint2 moment     = uint2(0, Xorshift::xorshift32(index.x * index.y)); //!< moment.x : 現在のsample数, moment.y : xiのoffset

	
	if (all(x >= 0.0f) && all(x <= 1.0f) && velocity.z <= 0.0f) {
		//!< pixelが範囲内の場合, 前の情報を格納
		reservoir = gReferenceReservoir.Load(uint3(sample_index, 0));
		moment    = gReferenceMoment.Load(uint3(sample_index, 0));

		// z値が0以外の場合, scaling関係でnoiseになってしまうのでリセットさせる
	}

	gReservoir[index] = reservoir;
	gMoment[index]    = moment;
	
}
