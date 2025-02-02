//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"
#include "../../Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gInput : register(u0); //!< input texture
RWTexture2D<float4> gIntermediate : register(u1); //!< intermediate texture
RWTexture2D<float4> gOutput : register(u2); //!< output texture

SamplerState gSampler : register(s0);

ConstantBuffer<Camera> gCamera : register(b0);

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
// variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float pi_v = 3.14159265359f;
static const int kKernelRadius = 15;

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float Gaussian2d(float2 diff, float sigma) {
	float a = 1.0f / (2.0f * pi_v * sigma * sigma);
	float b = exp(-(diff.x * diff.x + diff.y * diff.y) / (2.0f * sigma * sigma));
	return a * b;
}

float CrossBilateralWeight(float r, float ddiff, inout float weightTotal) {
	float w = exp(-r * r * 2.0f * pi_v) * (ddiff < gParameter.filter ? 1.0 : 0.0);
	weightTotal += w;
	return w;
}

// Perform a gaussian blur in one direction
float Blur(float2 texcoord) {
	float weightTotal = 1.0;
	float aoDepth = gIntermediate[texcoord].r;
	float totalAO = aoDepth.x;
	float centerZ = gPosition[texcoord].z;
	
	static const int kKernelRadius = 15;
	// [unroll]
	for (int i = -kKernelRadius; i < kKernelRadius; i++) {
		float2 current = texcoord + (float(i) * 2.0f);
		
		float sampleAO = gIntermediate[current].r;
		float sampleZ = gPosition[current].z;
		
		float diff   = abs(sampleZ - centerZ);
		float weight = CrossBilateralWeight(float(i), diff, weightTotal);
		totalAO     += sampleAO * weight;
	}

	return totalAO / weightTotal;
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
	
	float2 texcoord = float2(index) / float2(size);
	
	float blur = Blur(texcoord);
	
	float4 input = gInput[index];
	
	gOutput[index] = float4(input.rgb * blur, input.a);
	
}
