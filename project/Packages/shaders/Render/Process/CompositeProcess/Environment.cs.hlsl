//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Process.hlsli"
#include "../../DeferredBuffers.hlsli"

//* component
#include "../../../Component/CameraComponent.hlsli"
#include "../../../Component/SkyLightComponent.hlsli"

//* library
#include "../../../Library/ACES.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<CameraComponent> gCamera : register(b0);

SamplerState gSampler : register(s0);
ConstantBuffer<SkyLightComponent> gParameter : register(b1);

RWTexture2D<float4> gOutput : register(u0);

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
	// FIXME: Environment描画時, 明らかに負荷がかかる

	float2 d         = (index.xy + 0.5f) / size.xy * 2.0f - 1.0f;
	float3 target    = mul(float4(d.x, -d.y, 1.0f, 1.0f), gCamera.projInv).xyz;
	float3 direction = mul(float4(target, 0.0f), gCamera.world).xyz;

	float4 color = gParameter.GetEnvironment(gSampler, direction);
	color.rgb = ACES::IDT_sRGB_AP1(color.rgb);

	gOutput[index] = color;
}
