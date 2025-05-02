/*
# reference
 - https://dev.epicgames.com/documentation/en-us/unreal-engine/cinematic-depth-of-field-in-unreal-engine
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"
#include "../DeferredBuffers.hlsli"
#include "../../Library/Gaussian.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput : register(t0); //!< input texture
SamplerState gSampler    : register(s0);

RWTexture2D<float4> gOutput : register(u0); //!< output texture

ConstantBuffer<CameraComponent> gCamera : register(b0);

struct Parameter {
	float focalLength;
	float focalDistance;
	float f;
};
ConstantBuffer<Parameter> gParameter : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

/*
# reference
 - https://shikihuiku.github.io/post/projection_matrix/
*/
float FixLinearDepth(float depth) {
	return depth / (gCamera.far / gCamera.near - depth * (gCamera.far / gCamera.near - 1.0f));
}

float FixedViewDepth(float depth, uint2 index) {
	float2 d    = (index + 0.5f) / gConfig.size * 2.0f - 1.0f;
	float3 view = mul(float4(d.x, -d.y, 1.0f, 1.0f), gCamera.projInv).xyz;

	return view.z;
}

float CalculateCircleOfConfusion(float depth) {
	float cocRadius
		= gParameter.f * gParameter.focalLength * (gParameter.focalDistance - depth) / (depth * (gParameter.focalDistance - gParameter.focalLength));

	return saturate(abs(cocRadius));
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	Surface surface;
	surface.GetSurface(index);

	gOutput[index] = float4(surface.depth, 0.0f, 0.0f, 1.0f);
	
}

