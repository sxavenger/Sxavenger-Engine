//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../DeferredBuffers.hlsli"
#include "../../../Component/CameraComponent.hlsli"
#include "../../../Library/ACES.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// common define
////////////////////////////////////////////////////////////////////////////////////////////

#define _NUM_THREADS_X 16
#define _NUM_THREADS_Y 16

//=========================================================================================
// buffers
//=========================================================================================

struct Config {
	uint2 size;
};
ConstantBuffer<Config> gConfig : register(b0);
ConstantBuffer<CameraComponent> gCamera : register(b1);

TextureCube<float4> gEnvironment : register(t0);
SamplerState gSampler : register(s0);

struct Parameter {
	float intensity;
};
ConstantBuffer<Parameter> gParameter : register(b2);

RWTexture2D<float4> gOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

bool CheckOverTexture(uint2 index) {
	return any(index >= gConfig.size);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;

	Surface surface = (Surface)0;
	if (surface.GetSurface(index)) {
		return; //!< objectが描画されていないとこを描画
	}

	float2 d         = (index.xy + 0.5f) / gConfig.size.xy * 2.0f - 1.0f;
	float3 target    = mul(float4(d.x, -d.y, 1.0f, 1.0f), gCamera.projInv).xyz;
	float3 direction = mul(float4(target, 0.0f), gCamera.world).xyz;

	float4 color = gEnvironment.SampleLevel(gSampler, direction, 0.0f);
	color.rgb *= gParameter.intensity; // 環境光の強度を調整
	color.rgb = ACES::IDT_sRGB_AP1(color.rgb);

	gOutput[index] = color;
}
