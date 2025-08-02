//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../../Component/GPUParticleComponent.hlsli"
#include "../../../Component/TransformComponent.hlsli"

//* library
#include "../../../Library/Random.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* emitter
ConstantBuffer<TransformComponent> gTransform : register(b0);

struct Emitter {
	float3 seed;
	uint count;

	
	float radius;
};
ConstantBuffer<Emitter> gEmitter : register(b1);

//* particle
RWStructuredBuffer<GPUParticleComponent> gParticles : register(u0);

cbuffer Dimension : register(b2) {
	uint dimension;
};

//* free index
ConsumeStructuredBuffer<uint> gConsumeFreeIndex : register(u1);
RWStructuredBuffer<uint2> gCounter              : register(u2);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

bool GetFreeIndex(out uint index) {

	// consumeのsizeを取得
	uint size = gCounter[0].x;

	if (size > 0) { //!< emptyではない
		index = gConsumeFreeIndex.Consume();
		return true;
	}

	uint count = gCounter[0].y;

	if (count < dimension) {
		index = count;
		gCounter[0].y = count + 1;
		return true;
	}

	index = 0;
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(1, 1, 1)]
void main() {

	Random random;
	random.seed = gEmitter.seed;

	for (uint i = 0; i < gEmitter.count; i++) {

		uint index;
		if (!GetFreeIndex(index)) {
			break;
		}
		
		GPUParticleComponent element = (GPUParticleComponent)0;
		element.scale       = random.Random1d() * 2.0f;

		float3 size = float3(12.0f, 4.0f, 12.0f);
		
		element.translate = gTransform.GetPosition() + lerp(-size * 0.5f, size * 0.5f, random.Random3d());
		element.albedo      = float3(1.0f, 1.0f, 1.0f);
		element.transparent = 1.0f;
		element.lifeTime    = 2.0f + random.Random1d() * 2.0f;

		gParticles[index] = element;
	}
	
	
	
}
