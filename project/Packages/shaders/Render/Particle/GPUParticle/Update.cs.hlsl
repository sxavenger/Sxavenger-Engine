//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Component/GPUParticleComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* particle
RWStructuredBuffer<GPUParticleComponent> gParticles : register(u0);

cbuffer Dimension : register(b0) {
	uint dimension;
}

//* free index
AppendStructuredBuffer<uint> gAppendFreeIndex : register(u1);

//* parameter
cbuffer Time : register(b1) {
	float deltaTime;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint index = dispatchThreadId.x;

	if (index >= dimension) {
		return;
	}

	GPUParticleComponent element = gParticles[index];
	element.timer += deltaTime;

	if (element.lifeTime == 0.0f) {
		return;
	}

	if (element.timer >= element.lifeTime) {
		gParticles[index] = (GPUParticleComponent)0;
		gAppendFreeIndex.Append(index);
		return;
	}

	// test
	element.transparent = 1.0f - saturate(element.timer / element.lifeTime);

	gParticles[index] = element;
}
