//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"
#include "../../Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gInput        : register(u0); //!< input texture
Texture2D<float4> gIntermediate : register(t1); //!< intermediate texture
RWTexture2D<float4> gOutput       : register(u2); //!< output texture

SamplerState gSampler : register(s0);

ConstantBuffer<Camera> gCamera : register(b0);

struct Parameter {
	float radius;
	float maxRadius;
	float angleBias;
	float stregth;
	float filter;
	float scale;
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

float CrossBilateralWeight(float r, float ddiff, inout float weightTotal) {
	
	float w = exp(-r * r * pi_v) * (ddiff < gParameter.filter ? 1.0 : 0.0);
	weightTotal += w;
	return w;
}

float2 Blur(float2 uv, float2 scale) {
	float2 centerCoord = uv;
	float weightTotal = 1.0;

	float2 aoDepth = gIntermediate.SampleLevel(gSampler, centerCoord, 0).xy;	

	float totalAO = aoDepth.x;
	float centerZ = mul(float4(gPosition.SampleLevel(gSampler, centerCoord, 0).rgb, 1.0f), gCamera.view).z;
	
	// [unroll]
	for (int i = -kKernelRadius; i < kKernelRadius; ++i) {
		float2 texcoord  = centerCoord + float(i) * scale;
		float2 sampleAOZ = gIntermediate.SampleLevel(gSampler, texcoord, 0).xy;
		float z = mul(float4(gPosition.SampleLevel(gSampler, texcoord, 0).rgb, 1.0f), gCamera.view).z;
		float diff       = abs(z - centerZ);
		
		float weight = CrossBilateralWeight(float(i), diff, weightTotal);
		totalAO += sampleAOZ.x * weight;
	}

	return float2(totalAO / weightTotal, centerZ);
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
	
	float2 uv = index / float2(size);
	
	float2 blur = Blur(uv, gParameter.scale);
	
	float4 input = gInput[index];
	
	gOutput[index] = input * float4(blur.r, blur.r, blur.r, 1.0f);
	
	
}
