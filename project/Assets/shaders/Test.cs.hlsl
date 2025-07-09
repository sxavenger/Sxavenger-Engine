#include "../../Packages/shaders/Library/SDF.hlsli"

RWTexture2D<float4> gOutput : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint width, height;
	gOutput.GetDimensions(width, height);

	uint2 index = dispatchThreadId.xy;

	if (any(index >= uint2(width, height))) {
		return;
	}

	float2 uv = (float2(index) + 0.5f) / float2(width, height) * 2.0f - 1.0f;

	float3 size = float3(0.5f, 0.5f, 0.5f); //!< Boxのサイズ

	float distance = SDF::Box(float3(uv, 0.0f), size);

	gOutput[index] = float4(distance, -distance, 0.0f, 1.0f);
	
}
