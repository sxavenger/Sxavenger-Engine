//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PathtracingCommon.hlsli"

//* library
#include "../../../Library/BRDF.hlsli"
#include "../../../Library/ACES.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
RAYGENERATION void mainRaygeneration() {

	uint2 index     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	uint p = ReSTIR::FlattenIndex(index, dimension); //!< 2次元配列のindexを1次元配列のindexに変換

	ReSTIR::Moment moment = gMoment[p];

	if (moment.offset == 0) {
		moment = ReSTIR::Moment::GetDefault(index);
	}

	ReSTIR::Reservoir reservoir = (ReSTIR::Reservoir)0;

	GBuffer::Surface surface;
	if (!surface.FetchSurface(GBuffer::FetchArgument::Create(index, dimension, gCamera.projInv, gCamera.world))) { //!< surfaceが存在しない場合
		gInitalReservoir[p] = reservoir;
		gMoment[p]          = ReSTIR::Moment::GetDefault(index);
		return;
	}

	if (moment.CheckCompleteSample(maxSampleCount)) {
		gInitalReservoir[p] = reservoir;
		return; //!< サンプリング終了
	}

	Random random;
	random.seed = seed * uint3(index + 1, 1);

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position); //!< cameraからの方向ベクトルを取得

	uint loopCount = min(samplesPerFrame, maxSampleCount - moment.count);

	for (uint i = 0; i < loopCount; ++i) {

		float2 xi = Hammersley(moment.GetRandamizeSampleIndex(i, maxSampleCount), maxSampleCount);

		//!< GGX Importance Sample を使用
		float3 wi = ImportanceSampleGGX(xi, surface.roughness, surface.normal);
		float pdf = ImportanceSampleGGXPDF(wi, surface.roughness, surface.normal, v);

		RayDesc desc;
		desc.Origin    = surface.position;
		desc.Direction = wi;
		desc.TMin      = kTMin;
		desc.TMax      = kTMax;

		Payload payload = Payload::TracePrimaryRay(desc, kFlag);

		//* Lightの情報を取得
		float3 l = wi; //!< lightからの方向ベクトルを取得

		//* cameraからの方向ベクトルを取得
		float3 v = normalize(gCamera.GetPosition() - surface.position); //!< cameraからの方向ベクトルを取得

		BxDFAlbedo albedo   = BxDFAlbedo::Create(surface.albedo, surface.metallic);
		BxDFContext context = BxDFContext::Create(surface.normal, v, l);

		float3 color = EvaluateBRDF(albedo, context, surface.roughness) * context.NdotL * payload.lo;

		ReSTIR::Sample sample;
		sample.lo  = color;
		sample.ns  = payload.normal;
		sample.xs  = payload.position;
		sample.pdf = pdf;

		float w = dot(sample.lo, ACES::AP1_RGB2Y) / max(pdf, Mathmatic::kEpsilon);

		reservoir.Update(sample, w, random.Generate1d());
		
	}

	//!< reservoirの正規化
	reservoir.weight /= max(reservoir.m, 1.0f);
	gInitalReservoir[p] = reservoir;

	//!< momentの更新
	moment.count += loopCount;
	gMoment[p]    = moment;
	
}

