//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "SkyVisibilityCommon.hlsli"

//* component
#include "../../../Component/LightComponentCommon.hlsli"
#include "../../../Component/SkyLightComponent.hlsli"
#include "../../../Component/SkyAtmosphereComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWStructuredBuffer<ReservoirLib::Reservoir> gInitialReservoir : register(u0);

RaytracingAccelerationStructure gScene : register(t0);

ConstantBuffer<SkyLightComponent> gParameter : register(b0); //!< TestでAtmosphereを使用.

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
		gScene,
		0,
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
	float2 xi        = Hammersley((p + random.seed.y ^ ~random.seed.z) % kMaxSampleCount, kMaxSampleCount); //!< Test実行でランダムにする.
	float3 direction = ImportanceSampleCosineWeight(xi, surface.normal);
	float pdf        = ImportanceSampleCosineWeightPDF(direction, surface.normal);

	//!< 空との可視性判定
	bool isIntersect = IntersectionScene(surface.position, direction);

	//* 直接光の放射輝度を計算
	float3 radiance = isIntersect ? float3(0.0f, 0.0f, 0.0f) : gParameter.GetEnvironment(gSampler, direction).rgb;
	float3 l        = direction;

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position); //!< cameraからの方向ベクトルを取得

	BxDFAlbedo albedo   = BxDFAlbedo::Create(surface.albedo, surface.metallic);
	BxDFContext context = BxDFContext::Create(surface.normal, v, l);

	float3 color = EvaluateBRDF(albedo, context, surface.roughness) * context.NdotL * radiance * surface.ao / max(pdf, Mathmatic::kEpsilon);

	//!< Sampleの作成
	ReservoirLib::Sample sample = (ReservoirLib::Sample)0;
	sample.direction = direction;
	sample.radiance  = color;
	sample.pdf       = pdf;

	// 重み
	float w = dot(sample.radiance, ACES::AP1_RGB2Y);

	reservoir.Update(sample, w, random.Generate1d());

	// p_hat
	float p_hat = dot(reservoir.sample.radiance, ACES::AP1_RGB2Y);
	reservoir.w = (p_hat > 0.0f && reservoir.m > 0) ? reservoir.weight / (reservoir.m * p_hat) : 0.0f;

	gInitialReservoir[p] = reservoir;

}
