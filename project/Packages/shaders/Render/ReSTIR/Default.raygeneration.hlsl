//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PathtracingCommon.hlsli"

//* library
#include "../../Library/BRDF.hlsli"
#include "../../Library/ACES.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_RAYGENERATION void mainRaygeneration() {

	uint2 index     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	uint p = index.x + index.y * dimension.x; //!< 1次元配列のindex

	Moment moment = gMoment[p];

	moment.CheckOffset(index);

	Reservoir reservoir = (Reservoir)0;

	DeferredSurface surface;
	if (!surface.GetSurface(gDeferredBufferIndex.Get(), index)) {
		gInitalizeReservoir[p] = reservoir;
		gMoment[p]             = Moment::GetDefault(index);
		return; //!< surfaceが存在しない
	}

	if (moment.CheckCompleteSample(maxSampleCount)) {
		gInitalizeReservoir[p] = reservoir;
		return; //!< サンプリング終了
	}

	Random random;
	random.seed = seed * uint3(index + 1, 1);

	static const float3 kMinFrenel = float3(0.04f, 0.04f, 0.04f); //!< 非金属の最小Frenel値

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position); //!< cameraからの方向ベクトルを取得

	for (uint i = 0; i < min(samplesPerFrame, maxSampleCount - moment.index); ++i) {

		float2 xi = Hammersley(moment.GetRandamizeSampleIndex(i, maxSampleCount), maxSampleCount);

		//!< GGX Importance Sample を使用
		float3 wi = ImportanceSampleGGX(xi, surface.roughness, surface.normal);
		float pdf = ImportanceSampleGGXPDF(wi, surface.roughness, surface.normal, v);

		RayDesc desc;
		desc.Origin    = surface.position;
		desc.Direction = wi;
		desc.TMin      = kTMin;
		desc.TMax      = kTMax;

		Payload payload = TracePrimaryRay(desc, kFlag);

		Sample sample;
		sample.lo  = payload.lo;
		sample.xv  = surface.position;
		sample.nv  = surface.normal;
		sample.xs  = payload.position;
		sample.ns  = payload.normal;
		sample.pdf = pdf;

		float w = dot(sample.lo, ACES::AP1_RGB2Y) * rcp(max(pdf, kEpsilon));

		reservoir.Update(sample, w, random.Generate1d());
		
	}

	//!< reservoirの正規化
	reservoir.weight *= rcp(max(reservoir.m, 1.0f));
	gInitalizeReservoir[p] = reservoir;

	//!< momentの更新
	moment.index += min(samplesPerFrame, maxSampleCount - moment.index);
	gMoment[p]    = moment;
	
}
