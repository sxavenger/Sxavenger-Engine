//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyVisibilityCommon.hlsli"

//* component
#include "../../Component/LightComponentCommon.hlsli"
#include "../../Component/SkyLightComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWStructuredBuffer<ReservoirLib::Reservoir> gInitialReservoir : register(u0);

RaytracingAccelerationStructure gScene : register(t0);

ConstantBuffer<SkyLightComponent> gSkyLight : register(b0);

SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

bool IntersectionScene(float3 origin, float3 direction) {
	
	RayDesc desc;
	desc.Origin    = origin;
	desc.Direction = direction;
	desc.TMin      = kShadowDefaultTMin;
	desc.TMax      = kShadowDefaultTMax;

	RayQuery<0> q;

	q.TraceRayInline(
		scene,
		flag,
		kShadowMask,
		desc
	);

	if (q.Proceed()) {
		return true;
	}

	return false;

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

	uint p = ReservoirLib::FlattenIndex(pixel, dimension); //!< 2次元配列のindexを1次元配列のindexに変換

	ReservoirLib::Reservoir reservoir = (ReservoirLib::Reservoir)0;
	
	GBuffer::Surface surface;
	if (!surface.FetchSurface(GBuffer::FetchArgument::Create(pixel, dimension, gCamera.projInv, gCamera.world))) { //!< surfaceが存在しない場合
		gInitialReservoir[p] = reservoir;
		return;
	}

	Random random;
	random.seed = seed * uint3(pixel + 1, 1);

	//!< directionの決定
	float2 xi        = Hammersley(p % kMaxSampleCount, kMaxSampleCount);
	float3 direction = ImportanceSampleCosineWeight(xi, surface.normal);
	float pdf        = ImportanceSampleCosineWeightPDF(direction, surface.normal);

	//!< 空との可視性判定
	bool isIntersect = IntersectionScene(surface.position, direction);

	//!< Sampleの作成
	ReservoirLib::Sample sample = (ReservoirLib::Sample)0;
	sample.direction = direction;
	sample.radiance  = isIntersect ? float3(0.0f) : gSkyLight.GetEnvironment(gSampler, direction);
	sample.pdf       = pdf;

	//!< reservoirの更新
	float w = dot(sample.radiance, ACES::AP1_RGB2Y) / max(pdf, Mathmatic::kEpsilon);
	reservoir.Update(sample, w, random.Generate1d());

	//!< reservoirの正規化
	reservoir.weight /= max(reservoir.m, 1.0f);
	gInitialReservoir[p] = reservoir;
	
}
