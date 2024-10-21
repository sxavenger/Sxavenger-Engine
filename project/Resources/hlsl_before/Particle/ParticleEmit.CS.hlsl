//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Particle.hlsli"
#include "../Random.hlsli"

//=========================================================================================
// Buffers
//=========================================================================================
struct Emitter {
	float3 position;     //!< 位置
	float radius;        //!< 射出半径
	uint count;          //!< 発射数
	float frequency;     //!< 発射間隔
	float frequencyTime; //!< 発射間隔調整用時間
	int isEmit;          //!< 発射許可(bool)
};
ConstantBuffer<Emitter> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

struct Information {
	uint kParticleNum;
};
ConstantBuffer<Information> gInfo : register(b2);

//=========================================================================================
// RWBuffers
//=========================================================================================
RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int> gCounter       : register(u1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(1, 1, 1)] //!< emitter分のthreadを起動する
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	if (!gEmitter.isEmit) { //!< 発射許可が出てない
		return;
	}
	
	Random generater;
	generater.seed = (dispatchThreadId + gPerFrame.deltaTime) * gPerFrame.deltaTime;
	
	for (uint i = 0; i < gEmitter.count; ++i) {
		
		int particleIndex;
		InterlockedAdd(gCounter[0], 1, particleIndex);
		
		if (particleIndex < gInfo.kParticleNum) {
			gParticle[particleIndex].scale     = generater.Generate3d();
			gParticle[particleIndex].translate = generater.Generate3d();
			gParticle[particleIndex].velocity = generater.Generate3d();
			gParticle[particleIndex].lifeTime  = 10.0f;
			gParticle[particleIndex].color.rgb = generater.Generate3d();
			gParticle[particleIndex].color.a   = 1.0f;
		}

	}
}