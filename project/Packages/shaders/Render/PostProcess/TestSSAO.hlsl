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
	
	Surface surface;
	
	if (!surface.GetSurface(index)) {
		return;
	}
	
	float sum = 0.0f;
	float count = 0.0f;
	
	static const int2 kStrength = uint2(gParameter.stregth, gParameter.stregth);
	
	for (int x = -kStrength.x; x <= kStrength.x; ++x) {
		for (int y = -kStrength.y; y <= kStrength.y; ++y) {
			
			if (x == 0 && y == 0) {
				continue;
			}
			
			int2 ind = int2(index) + int2(x, y);
			
			Surface s;
			if (!s.GetSurface(ind)) {
				continue;
			}
			
			count += 1.0f;
			
			if (length(surface.position - s.position) > gParameter.radius) {
				continue;
			};
			
			float d = saturate(dot(s.normal, surface.normal));
			sum += d;
		}
	}
	
	float occ = saturate(sum / count);
	
	gOutput[index] = float4(occ, occ, occ, 1.0f);
	
}
