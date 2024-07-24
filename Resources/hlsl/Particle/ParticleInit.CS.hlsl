//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Particle.hlsli"

//=========================================================================================
// Buffers
//=========================================================================================
struct Information {
	uint kParticleNum;
};
ConstantBuffer<Information> gInfo : register(b0);

//=========================================================================================
// Output
//=========================================================================================
RWStructuredBuffer<Particle> gOutput : register(u0);

//=========================================================================================
// configs
//=========================================================================================
#define NUMTHREADS_X 1024
#define NUMTHREADS_Y 1
#define NUMTHREADS_Z 1

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUMTHREADS_X, NUMTHREADS_Y, NUMTHREADS_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	if (dispatchThreadId.x >= gInfo.kParticleNum) { //!< paritlceの最大数を超えていた場合
		return;
	}
	
	Particle particle = (Particle)0;
	
	particle.scale = float3(0.5f, 0.5f, 0.5f);
	particle.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	// ...初期化処理
	
	gOutput[dispatchThreadId.x] = particle;
	
}