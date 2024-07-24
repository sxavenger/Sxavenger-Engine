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

//=========================================================================================
// RWBuffers
//=========================================================================================
RWStructuredBuffer<Particle> gParticle : register(u0);

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
		gParticle[i].scale     = generater.Generate3d();
		gParticle[i].translate = generater.Generate3d();
		gParticle[i].color.rgb = generater.Generate3d();
		gParticle[i].color.a   = 1.0f;

	}
}