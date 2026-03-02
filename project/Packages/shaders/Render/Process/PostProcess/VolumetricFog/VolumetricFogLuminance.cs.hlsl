//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "VolumetricFog.hlsli"

//* component
#include "../../../../Component/CameraComponent.hlsli"
#include "../../../../Component/TransformComponent.hlsli"
#include "../../../../Component/DirectionalLightComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* textures
Texture2D<float4> gInput : register(t0);
RWTexture2D<float4> gOutput : register(u0);

//* G-buffer
Texture2D<float4> gPosition : register(t1);

//* camera
ConstantBuffer<CameraComponent> gCamera : register(b1);

//* scene
RaytracingAccelerationStructure gScene : register(t2);

//* light
struct LightCount {
	uint count;
};

//!< Directional Light
ConstantBuffer<LightCount> DirectionalLightCount                        : register(b0, space1); //!< 32bitconstants
StructuredBuffer<TransformComponent> gDirectionalLightTransforms        : register(t0, space1);
StructuredBuffer<DirectionalLightComponent> gDirectionalLightParameters : register(t1, space1);

//// Point Light
//ConstantBuffer<LightCount> PointLightCount                 : register(b1, space1);
//StructuredBuffer<TransformComponent> gPointLightTransforms : register(t2, space1);
//StructuredBuffer<PointLightComponent> gPointLights         : register(t3, space1);

//// Spot Light
//ConstantBuffer<LightCount> SpotLightCount                 : register(b2, space1);
//StructuredBuffer<TransformComponent> gSpotLightTransforms : register(t4, space1);
//StructuredBuffer<SpotLightComponent> gSpotLights          : register(t5, space1);

// TODO: Directional Lightだけで観測

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 CalculateLuminance(float3 position) {

	float3 luminance = float3(0.0f, 0.0f, 0.0f);

	for (uint i = 0; i < DirectionalLightCount.count; ++i) {
		
		DirectionalLightComponent light = gDirectionalLightParameters[i];
		TransformComponent transform    = gDirectionalLightTransforms[i];

		//!< Lightの影響範囲
		float3 color_mask = light.GetColorMask();
		float light_mask  = light.GetLightMask(gScene, transform.GetDirection(), position);

		luminance += color_mask * light_mask;
	}

	return luminance;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispathThreadId : SV_DispatchThreadID) {
	
	uint2 pixel = dispathThreadId.xy;
	
	if (CheckOverTexture(pixel)) {
		return;
	}

	float3 camera_position = gCamera.GetPosition();
	float3 world_position  = gPosition.Load(uint3(pixel, 0)).xyz;
	
	float3 direction = normalize(world_position - camera_position);
	float distance   = length(world_position - camera_position);

	static const uint kStepCount = 16;

	float step       = (gParameter.viewDistance - gParameter.startDistance) / kStepCount;
	float3 luminance = float3(0.0f, 0.0f, 0.0f);

	for (uint i = 1; i <= kStepCount; ++i) {

		float3 t = direction * (gParameter.startDistance + step * i);

		if (length(t) > distance) {
			break;
		}
		
		luminance += CalculateLuminance(camera_position + t) / kStepCount;
	}

	gOutput[pixel] = gInput[pixel] + float4(luminance, 0.0f);

	// TODO: Weightの導入
	
}
