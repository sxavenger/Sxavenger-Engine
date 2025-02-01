//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"
#include "../../Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gInput  : register(u0); //!< input texture
RWTexture2D<float4> gOutput : register(u1); //!< output texture

ConstantBuffer<Camera> gCamera : register(b0);

SamplerState gSampler : register(s0);

struct Parameter {
	float radius;
	float maxRadius;
	float angleBias;
	float stregth;
	float filter;
};
ConstantBuffer<Parameter> gParameter : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// common variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float pi_v = 3.14159265359f;

// vvv parameter vvv

static const uint kNumDirection = 8;
static const uint kNumSteps     = 8;

static const float kTanAngleBias = tan(gParameter.angleBias);

static const uint kMaxSteps = 16;

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

float2 PseudoRandom(float2 uv) {
	float2 p = float2(dot(uv, float2(127.1, 311.7)), dot(uv, float2(269.5, 183.3)));
	return frac(sin(p) * 43758.5453123) * 2.0 - 1.0;
}

// vvv HBAO vvv

float2 Rand(float2 p) {
	p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
	return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
}


float3 GetViewPosition(float2 uv) {
	float3 position = gPosition.SampleLevel(gSampler, uv, 0).xyz;
	return mul(float4(position, 1.0f), gCamera.view).xyz;
}

float3 MinDiff(float3 a, float3 b, float3 c) {
	float3 v1 = b - a;
	float3 v2 = a - c;
	
	return dot(v1, v1) < dot(v2, v2) ? v1 : v2;
}

float3 RandCosSinJitter(float2 uv) {
	float2 r = Rand(uv); //!< rand
	float angle = 2.0f * pi_v * r.x / float(kNumDirection);
	return float3(cos(angle), sin(angle), r.y);
}

void CalculateNumSteps(inout float2 stepSize, inout float numStep, float radius, float rand) {
	float maxRadius = gParameter.maxRadius;
	float2 invRes   = 1.0f / float2(gConfig.size);
	
	numStep = min(float(kNumSteps), radius);

	float stepSizeInPixels = radius / (numStep + 1.0);
	float maxNumSteps      = maxRadius / stepSizeInPixels;
	
	if (maxNumSteps < numStep) {
		numStep = floor(maxNumSteps + rand);
		numStep = max(numStep, 1.0);
		stepSizeInPixels = maxRadius / numStep;
	}

	stepSize = stepSizeInPixels * invRes.xy;
}

float2 RotateDirection(float2 dir, float cos, float sin) {
	return float2(dir.x * cos - dir.y * sin, dir.x * sin + dir.y * cos);
}

float2 SnapUVOffset(float2 uv) {
	float2 res    = float2(gConfig.size);
	float2 invRes = 1.0f / res;
	return round(uv * res) * invRes;
}

float Rsqrt(float x) {
	return 1.0f / sqrt(x);
}

float GetBiasTangent(float3 t) {
	return -t.z * Rsqrt(dot(t.xy, t.xy)) + kTanAngleBias;
}

float TanToSin(float x) {
	return x * Rsqrt(1.0f + x * x);
}

float GetTangent(float3 t) {
	return -t.z * Rsqrt(dot(t.xy, t.xy));
}

float falloffFactor(float d2) {
	float NegInvRadius = -1.0 / (gParameter.radius * gParameter.radius);
	return d2 * NegInvRadius + 1.0;
}

float IntegrateOcclusion(float2 uv0, float2 snapped_duv, float3 p, float3 dPdu, float3 dPdv, inout float tanH) {
	float ao = 0.0;

	float3 t    = snapped_duv.x * dPdu + snapped_duv.y * dPdv;
	float tanT  = GetBiasTangent(t);
	float sinT  = TanToSin(tanT);
	float3 s    = GetViewPosition(uv0 + snapped_duv);
	float3 diff = s - p;
	float tanS = GetTangent(diff);
	float sinS = TanToSin(tanS);
	float d2 = dot(diff, diff);
	float R2 = gParameter.radius * gParameter.radius; // R*R
	if ((d2 < R2) && (tanS > tanT)) {
		ao = falloffFactor(d2) * saturate(sinS - sinT);
		tanH = max(tanH, tanS);
	}

	return ao;
}

float CalculateHorizonOcclusion(float2 dUv, float2 texelDeltaUV, float2 uv0, float3 p, float numSteps, float randStep, float3 dPdu, float3 dPdv) {

	float ao = 0.0f;
	
	float2 uv      = uv0 + SnapUVOffset(randStep * dUv);
	float2 deltaUV = SnapUVOffset(dUv);
	float3 t       = deltaUV.x * dPdu + deltaUV.y * dPdv;
	
	float tanT = GetBiasTangent(t);

	
	float2 snappedUV = SnapUVOffset(randStep * deltaUV + texelDeltaUV);
	ao = IntegrateOcclusion(uv0, snappedUV, p, dPdu, dPdv, tanT);
	numSteps--;
	
	float sinH = TanToSin(tanT);
	
	for (int j = 1; j < kMaxSteps; ++j) {
		if (float(j) >= numSteps) {
			break;
		}

		uv += deltaUV;
		float3 s = GetViewPosition(uv);
		float3 diff = s - p;
		float tanS = GetTangent(diff);
		float d2 = dot(diff, diff);
		float R2 = gParameter.radius * gParameter.radius;
		
		if ((d2 < R2) && (tanS > tanT)) {
			float sinS = TanToSin(tanS);
			ao += falloffFactor(d2) * saturate(sinS - sinH);

			tanT = tanS;
			sinH = sinS;
		}
	}

	return ao;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	uint2 size = gConfig.size;
	
	if (any(index >= size)) {
		return; //!< texture size over
	}
	
	Surface surface;
	
	const float4 not_process = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	if (!surface.GetSurface(index)) {
		gOutput[index] = not_process;
		return; //!< surface is not valid.
	}
	

	float3 position = mul(float4(surface.position, 1.0f), gCamera.view).xyz;
	float3 normal   = normalize(mul(surface.normal, (float3x3)gCamera.view));
	
	float diskRadius = gParameter.radius * gCamera.proj._11 * 0.5f / position.z;
	float radius     = diskRadius * size.x;
	
	if (radius <= 1.0f) {
		gOutput[index] = not_process;
		return;
	}
	
	float2 uv     = index / float2(size);
	float2 res    = float2(size);
	float2 invRes = 1.0f / float2(size);
	
	float3 pr = GetViewPosition(uv + float2(invRes.x, 0.0f));
	float3 pl = GetViewPosition(uv - float2(invRes.x, 0.0f));
	float3 pt = GetViewPosition(uv + float2(0.0f, invRes.y));
	float3 pb = GetViewPosition(uv - float2(0.0f, invRes.y));
	
	
	float3 dPdu = MinDiff(position, pr, pl);
	float3 dPdv = MinDiff(position, pt, pb) * (res.x * invRes.x); //!< ? 

	float ao = 0.0f;
	
	const float alpha = pi_v * 2.0f / float(kNumDirection);
	
	float numStep;  //!< output
	float2 stepSize; //!< output
	float3 rand = RandCosSinJitter(uv);
	CalculateNumSteps(stepSize, numStep, radius, rand.z);
	
	for (uint i = 0; i < kNumDirection; ++i) {

		float angle = alpha * float(i);
		float2 dir  = RotateDirection(float2(cos(angle), sin(angle)), rand.x, rand.y);
		float2 deltaUV = dir * stepSize;
		float2 texelDeltaUV = dir * invRes;
		ao += CalculateHorizonOcclusion(deltaUV, texelDeltaUV, uv, position, numStep, rand.z, dPdu, dPdv);
	}
	
	ao = 1.0f - ao / float(kNumDirection) * gParameter.stregth;
	
	gOutput[index] = float4(ao, position.z, 0.0f, 1.0f);
}
