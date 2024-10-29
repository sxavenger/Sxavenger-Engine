//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Particle.hlsli"
#include "../Random.hlsli"

//=========================================================================================
// Buffers
//=========================================================================================
struct Information {
	uint kParticleNum;
};
ConstantBuffer<Information> gInfo : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

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
	
	uint index = dispatchThreadId.x;
	
	if (index < gInfo.kParticleNum) {
		
		if (gOutput[index].color.a != 0.0f) { //!< alpha0は死んでいるとみなす
			
			gOutput[index].translate += gOutput[index].velocity * gPerFrame.deltaTime;
			gOutput[index].currentTime += gPerFrame.deltaTime;
			
			float alpha = 1.0f - (gOutput[index].currentTime / gOutput[index].lifeTime);
			gOutput[index].color.a = saturate(alpha);

		}
	}
	
}
