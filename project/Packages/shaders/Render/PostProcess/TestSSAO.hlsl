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
	float2 scale;
};
ConstantBuffer<Parameter> gParameter : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// common variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float pi_v = 3.14159265359f;

// vvv except vvv

static const float4 kNotProcessed = float4(0.0f, 0.0f, 0.0f, 0.0f);

// vvv parameter vvv

static const uint kStepCount = 16;
static const uint kNumDirections = 8;

static const float kTanBias = tan(gParameter.angleBias);

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

float2 PseudoRandom2(float2 uv) {
	float2 p = float2(dot(uv, float2(127.1, 311.7)), dot(uv, float2(269.5, 183.3)));
	return frac(sin(p) * 43758.5453123) * 2.0 - 1.0;
}

float3 PseudoRandom3(float2 uv) {
	float3 p = float3(dot(uv, float2(127.1, 311.7)), dot(uv, float2(269.5, 183.3)), dot(uv, float2(419.2, 371.9)));
	return frac(sin(p) * 43758.5453123) * 2.0 - 1.0;
}

// vvv SSAO vvv

float3 GetViewPosition(float2 texcoord) {

	float3 position = gPosition.SampleLevel(gSampler, texcoord, 0).xyz;
	float4 viewPosition = mul(float4(position, 1.0f), gCamera.view);
	return viewPosition.xyz / viewPosition.w;
	
}

float3 MinDiff(float3 p, float3 pr, float3 pl) {
	float3 v0 = pr - p;
	float3 v1 = p - pl;
	return (dot(v0, v0) < dot(v1, v1)) ? v0 : v1;
}

void CalculateNumStep(out float2 stepSizeInUV, out float numSteps, float radiusInPixels, float rand) {
	numSteps = min(float(kStepCount), radiusInPixels);
	float stepSizeInPixels = radiusInPixels / (numSteps + 1.0f);
	float maxNumSteps = gParameter.maxRadius / stepSizeInPixels;
	
	if (numSteps > maxNumSteps) {
		numSteps = floor(maxNumSteps + rand);
		numSteps = max(1.0f, numSteps);
		stepSizeInPixels = gParameter.maxRadius / numSteps;
	}
	
	float2 invRes = 1.0f / float2(gConfig.size);
	
	stepSizeInUV = stepSizeInPixels / invRes;
}

// vvv calculate occlusion vvv

float Tangent(float3 t) {
	return t.z * rsqrt(dot(t.xy, t.xy));
}

float BiasedTangent(float3 t) {
	return t.z * rsqrt(dot(t.xy, t.xy)) + kTanBias;
}

float TanToSin(float x) {
	return x * rsqrt(x * x + 1);
}

float FalloffFactor(float d2, float r2) {
	return 1.0f - d2 / r2;
}

float IntegrateOcclusion(float2 texcoord, float2 duv, float3 p, float3 dpdu, float3 dpdv, inout float tanH) {
	float ao = 0.0f;
	
	float3 t = duv.x * dpdu + duv.y * dpdv;
	float tanT = BiasedTangent(t);
	float sinT = TanToSin(tanT);
	float3 s = GetViewPosition(texcoord + duv);
	float3 diff = s - p;
	float tanS = Tangent(diff);
	float sinS = TanToSin(tanS);
	float d2 = dot(diff, diff);
	float r2 = gParameter.radius * gParameter.radius;
	
	if ((d2 < r2) && (tanS > tanT)) {
		ao = FalloffFactor(d2, r2) * saturate(sinS - sinT);
		tanH = max(tanH, tanS);
	}
	
	return ao;

}

float CalculateHorizonOcclusion(float2 deltaUV, float2 texelDeltaUV, float2 texcoord, float3 p, float numSteps, float randStep, float3 dpdu, float3 dpdv) {

	float ao = 0.0f;

	float2 uv = texcoord + randStep * deltaUV;
	float3 t = deltaUV.x * dpdu + deltaUV.y * dpdv;
	float tanH = BiasedTangent(t);
	
	// first step
	float2 duv = randStep * deltaUV + texelDeltaUV;
	ao = IntegrateOcclusion(texcoord, duv, p, dpdu, dpdv, tanH);
	--numSteps;
	
	float sinH = TanToSin(tanH);
	
	for (uint si = 1; si < kStepCount; ++si) {
		if (float(si) >= numSteps) {
			break;
		}
		
		uv += deltaUV;
		float3 s = GetViewPosition(uv);
		float3 diff = s - p;
		float tanS = Tangent(diff);
		float d2 = dot(diff, diff);
		float r2 = gParameter.radius * gParameter.radius;
		
		if ((d2 < r2) && (tanS > tanH)) {
			float sinS = TanToSin(tanS);
			ao += FalloffFactor(d2, r2) * saturate(sinS - sinH);

			tanH = tanS;
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
	uint2 size  = gConfig.size;
	
	if (any(index >= size)) {
		return; //!< texture size over
	}
	
	//* vvv HBAO vvv *//
	
	float2 texcoord = (float2(index)) / size;
	
	float3 p = GetViewPosition(texcoord);
	
	float focalLength    = gCamera.proj._22 * float(size.y) / float(size.x);
	float radiusInPixels = gParameter.radius * focalLength / p.z;
	
	if (radiusInPixels <= 1.0f) {
		gOutput[index] = kNotProcessed;
		return;
	}

	float2 res = float2(size);
	float2 invRes = 1.0f / res;
	
	float3 pr = GetViewPosition(texcoord + float2(invRes.x, 0.0f));
	float3 pl = GetViewPosition(texcoord - float2(invRes.x, 0.0f));
	float3 pt = GetViewPosition(texcoord + float2(0.0f, invRes.y));
	float3 pb = GetViewPosition(texcoord - float2(0.0f, invRes.y));

	float3 dpdu = MinDiff(p, pr, pl);
	float3 dpdv = MinDiff(p, pt, pb);

	float3 random = PseudoRandom3(texcoord * 0.24f);
	
	float numSteps;
	float2 stepSize;
	CalculateNumStep(stepSize, numSteps, radiusInPixels, random.z);
	
	float ao = 0.0f;
	
	for (uint d = 0; d < kNumDirections; ++d) {
		
		float k = (float(d) + random.x) / float(kNumDirections);
		float phi = k * pi_v * 2.0f;
		float2 dir = float2(cos(phi), sin(phi));
		float2 deltaUV      = dir * stepSize;
		float2 texelDeltaUV = dir * invRes;
		
		ao += CalculateHorizonOcclusion(deltaUV, texelDeltaUV, texcoord, p, numSteps, random.y, dpdu, dpdv);
	}
	
	ao = 1.0f - saturate(ao / float(kNumDirections) * gParameter.stregth);

	gOutput[index] = float4(ao, ao, ao, 1.0f);
}
