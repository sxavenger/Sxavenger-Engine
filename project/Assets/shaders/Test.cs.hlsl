//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Packages/shaders/Library/ACES.hlsli"
#include "../../Packages/shaders/Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* texture
RWTexture2D<float4> gOutput : register(u0);

TextureCube<float4> gEnvironment : register(t0);
SamplerState gSampler            : register(s0);

ConstantBuffer<CameraComponent> gCamera : register(b0);

struct Parameter {
	uint flag;
};
ConstantBuffer<Parameter> gParameter : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 size;
	gOutput.GetDimensions(size.x, size.y);

	uint2 index = dispatchThreadId.xy;

	if (any(index >= size)) {
		return; // Out of bounds check
	}

	float2 d          = (index.xy + 0.5f) / size.xy * 2.0f - 1.0f;
	float3 target    = mul(float4(d.x, -d.y, 1.0f, 1.0f), gCamera.projInv).xyz;
	float3 direction = mul(float4(target, 0.0f), gCamera.world).xyz;

	float3 color = gEnvironment.SampleLevel(gSampler, direction, 0.0f).rgb;

	if (gParameter.flag) {
		float3 aces = ACES::IDT_sRGB(color);
		float3 oces = ACES::RRT(aces);
		color = ACES::ODT_sRGB_D65(oces);
	}

	if (any(color > 1.0f)) {
		color = float3(0.0f, 0.0f, 0.0f);
	}
		 
	gOutput[index] = float4(color, 1.0f);
	
	
}
