//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Transition.hlsli"
#include "../../Component/CameraComponent.hlsli"
#include "../../Library/Random.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<uint3> gMoment             : register(u0);
RWTexture2D<float4> gReservoirDiffuse  : register(u1);
RWTexture2D<float4> gReservoirSpecular : register(u2);

Texture2D<uint3> gReferenceMoment             : register(t1);
Texture2D<float4> gReferenceReservoirDiffuse  : register(t2);
Texture2D<float4> gReferenceReservoirSpecular : register(t3);

Texture2D<float4> gVelocity : register(t0);

SamplerState gSampler : register(s0);

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

	float4 reservoir_diffuse  = float4(0, 0, 0, 0);
	float4 reservoir_specular = float4(0, 0, 0, 0);
	uint3 moment              = uint3(Xorshift::xorshift32(index.x * index.y), 0, 0);
	//!< x: Hammerselyのoffset y: sample数(diffuse) z: sample数(specular, 継承不可)

	if (all(velocity.xy == 0.0)) {
		reservoir_specular = gReferenceReservoirSpecular.Load(uint3(index, 0));
		moment.z           = gReferenceMoment.Load(uint3(index, 0)).z;
	}

	
	if (all(x >= 0.0f) && all(x <= 1.0f) && velocity.z <= 0.0f) {
		//!< pixelが範囲内の場合, 前の情報を格納
		reservoir_diffuse = gReferenceReservoirDiffuse.Load(uint3(sample_index, 0));
		moment.xy         = gReferenceMoment.Load(uint3(sample_index, 0)).xy;

		// z値が0以外の場合, scaling関係でnoiseになってしまうのでリセットさせる
	}

	gReservoirDiffuse[index]  = reservoir_diffuse;
	gReservoirSpecular[index] = reservoir_specular;
	gMoment[index]            = moment;
	
}
