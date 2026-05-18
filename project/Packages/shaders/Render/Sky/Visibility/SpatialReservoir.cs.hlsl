//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyVisibilityCommon.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWStructuredBuffer<ReservoirLib::Reservoir> gDestinationReservoir : register(u0);
StructuredBuffer<ReservoirLib::Reservoir> gSourceReservoir : register(t0);

cbuffer ATrous : register(b0) {
	uint recursion;
};

//=========================================================================================
// constant variables
//=========================================================================================

static const uint kNeighborCount = 8;
static const int2 kNeighborOffsets[kNeighborCount] = {
	int2(-1, -1), int2( 0, -1), int2( 1, -1),
	int2(-1,  0),               int2( 1,  0),
	int2(-1,  1), int2( 0,  1), int2( 1,  1),
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

bool IsValidPixel(uint2 pixel) {
	return all(pixel < dimension); //!< オーバーフロー前提で判定をとる.
}

float SurfaceSimilarity(GBuffer::Surface surfaceA, GBuffer::Surface surfaceB) {

	float weight = 1.0f;

	float depth_diff = abs(surfaceA.depth - surfaceB.depth);
	weight *= exp(-depth_diff * depth_diff * 100.0f);

	return weight;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return;
	}

	Random random;
	random.seed = seed * uint3(pixel + 1, 1);

	uint p = ReservoirLib::FlattenIndex(pixel, dimension);

	GBuffer::FetchArgument argument = GBuffer::FetchArgument::Create(pixel, dimension, gCamera.projInv, gCamera.world);

	GBuffer::Surface surface;
	if (!surface.FetchSurface(argument)) {
		gDestinationReservoir[p] = (ReservoirLib::Reservoir)0;
		return;
	}

	ReservoirLib::Reservoir reservoir = gSourceReservoir[p];

	for (uint i = 0; i < kNeighborCount; i++) {
		uint2 neighbor_pixel = pixel + kNeighborOffsets[i] * (1u << recursion); //!< 隣接ピクセルの座標を計算. recursionが大きいほど遠くのピクセルを参照する.
		if (!IsValidPixel(neighbor_pixel)) {
			continue;
		}

		uint neighbor_index = ReservoirLib::FlattenIndex(neighbor_pixel, dimension);

		GBuffer::FetchArgument neighbor_argument = GBuffer::FetchArgument::Create(neighbor_pixel, dimension, gCamera.projInv, gCamera.world);
		GBuffer::Surface neighbor_surface;
		if (!neighbor_surface.FetchSurface(neighbor_argument)) {
			continue;
		}

		ReservoirLib::Reservoir neighbor_reservoir = gSourceReservoir[neighbor_index];

		float attenuation = rcp(1.0f + recursion);

		float weight = SurfaceSimilarity(surface, neighbor_surface) * neighbor_reservoir.weight; //!< reservoirの重みとサンプル数を考慮して重みを計算.

		reservoir.Merge(neighbor_reservoir, weight, random.Generate1d());

	}

	reservoir.Normalize();
	
	gDestinationReservoir[p] = reservoir;

}
