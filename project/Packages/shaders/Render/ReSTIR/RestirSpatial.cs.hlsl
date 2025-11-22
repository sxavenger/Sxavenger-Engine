//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "RestirPass.hlsli"

//* library
#include "../../Library/ACES.hlsli"

//* content
#include "../../Content/Random.hlsli"

//* common
#include "../DeferredBufferIndex.hlsli"

//=========================================================================================
// constant variables
//=========================================================================================

static const uint kSpatialMaxMCount = 500; //!< spatial reservoirの最大サンプル数

//=========================================================================================
// buffer
//=========================================================================================

RWStructuredBuffer<Reservoir> gTemporalReservoir : register(u0);
RWStructuredBuffer<Reservoir> gSpatialReservoir  : register(u1);

//* deferred index
ConstantBuffer<DeferredBufferIndexConstantBuffer> gDeferredBufferIndex : register(b0);

//* scene
RaytracingAccelerationStructure gScene : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// structures
////////////////////////////////////////////////////////////////////////////////////////////
struct Set {

	float p_q;
	uint m;
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint2 GetNeighbor(uint2 p, float radius, float2 r) {
	// ランダムに円形の近傍をサンプリング
	float a = kTau * r.x;
	float d = radius * sqrt(r.y);

	int2 offset = int2(d * cos(a), d * sin(a));

	return p + offset;
}

bool GeometricSimilarity(DeferredSurface s1, DeferredSurface s2) {
	if (dot(s1.normal, s2.normal) < 0.9902680687f) { //!< cos(25deg)
		return false;
	}

	//if (abs(s1.depth - s2.depth) > 0.05 * s1.depth) {
	//	return false;
	//}

	if (s1.roughness != s2.roughness || s1.metallic != s2.metallic) {
		return false;
	}

	return true;
}

float ComputeJacobian(Reservoir rn, DeferredSurface q, DeferredSurface n) {
	 // サンプル方向ベクトル
	float3 v_q = rn.sample.xs - q.position;
	float3 v_n = rn.sample.xs - n.position;

	float dist_q2 = dot(v_q, v_q); // ||xq - xs||^2
	float dist_n2 = dot(v_n, v_n); // ||xqn - xs||^2

	// 角度 cos(phi)
	float cos_q = dot(q.normal, normalize(v_q));
	float cos_n = dot(n.normal, normalize(v_n));

	// 可視判定向け：負だと裏面（法線に対し背後）
	if (cos_q <= 0 || cos_n <= 0) {
		return 0.0f;
	}

	return (cos_q * rcp(cos_n)) * (dist_n2 * rcp(dist_q2));
}

bool IsVisible(float3 p1, float3 p2) {
	//!< p1からp2への可視判定

	static const float kTMin = 0.01f;

	RayDesc desc;
	desc.Origin    = p1;
	desc.Direction = normalize(p2 - p1);
	desc.TMin      = kTMin;
	desc.TMax      = length(p2 - p1) - kTMin;
	
#ifdef _INLINE_RAYTRACING
		RayQuery<0> q;

		q.TraceRayInline(
			gScene,
			RAY_FLAG_CULL_BACK_FACING_TRIANGLES,
			0b01,
			desc
		);

		if (q.Proceed()) {
			return false;
		}
#endif
	
	return true;
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREAD_X, _NUM_THREAD_Y, _NUM_THREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;

	if (any(pixel >= dimension)) {
		return; //!< 範囲外
	}

	DeferredSurface surface;
	if (!surface.GetSurface(gDeferredBufferIndex.Get(), pixel)) {
		return; //!< surfaceが存在しない
	}

	uint p       = Flatten(pixel);
	Reservoir rs = gSpatialReservoir[p];

	Random random;
	random.seed = seed * uint3(pixel + 1, 1);

	uint iteration = (rs.m < kSpatialMaxMCount / 2) ? 9 : 3;
	//!< 初期段階では多くの近傍を探索し, ある程度サンプルが集まったら少ない近傍で更新

	//!< mergeされた隣接ピクセルのindexを保存する配列
	Set numbers[9];
	uint count = 0;

	for (uint i = 0; i < iteration; ++i) {

		uint2 neighbor = GetNeighbor(pixel, 8.0f, random.Generate2d());

		if (any(neighbor >= dimension)) {
			continue; //!< 範囲外
		}

		DeferredSurface neighbor_surface;
		if (!neighbor_surface.GetSurface(gDeferredBufferIndex.Get(), neighbor)) {
			continue; //!< surfaceが存在しない
		}

		if (!GeometricSimilarity(surface, neighbor_surface)) {
			continue; //!< 幾何学的に類似していない
		}

		uint q = Flatten(neighbor);

		Reservoir rn = gTemporalReservoir[q];

		float j     = ComputeJacobian(rn, surface, neighbor_surface);
		float p_hat = dot(rn.sample.lo, ACES::AP1_RGB2Y);
		float p_q   = j > 0.0f ? p_hat * rcp(j) : 0.0f;

		if (all(rn.sample.xs != 0.0f) && !IsVisible(rs.sample.xv, rn.sample.xs)) {
			p_q = 0.0f;
		}

		rs.Merge(rn, p_q, random.Generate1d());

		Set set;
		set.p_q = p_q;
		set.m   = rn.m;
		numbers[count++] = set;

	}

	uint z = 0.0f;
	z += rs.m;
	
	for (uint i = 0; i < count; ++i) {
		if (numbers[i].p_q > 0.0f) {
			z += numbers[i].m;
		}
	}

	rs.w = all(rs.sample.lo != 0.0f) ? rs.weight * rcp(z * dot(rs.sample.lo, ACES::AP1_RGB2Y)) : 0.0f;
	gSpatialReservoir[p] = rs;
}
