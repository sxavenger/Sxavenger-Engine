//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../../Packages/shaders/Component/PointLightComponent.hlsli"
#include "../../../Packages/shaders/Component/CameraComponent.hlsli"
#include "../../../Packages/shaders/Component/TransformComponent.hlsli"

#define _NUM_THREADS_X 16
#define _NUM_THREADS_Y 16

//=========================================================================================
// buffers
//=========================================================================================

RWTexture3D<float4> gVoxel : register(u0);

ConstantBuffer<CameraComponent> gCamera : register(b0);

ConstantBuffer<PointLightComponent> gPointLight : register(b1);
ConstantBuffer<TransformComponent> gTransform   : register(b2);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint3 index = dispatchThreadId;

	uint3 dimension;
	gVoxel.GetDimensions(dimension.x, dimension.y, dimension.z); //!< HACK

	if (any(index >= dimension)) {
		return; //!< 範囲外
	}

	float2 d = (index.xy + 0.5f) / dimension.xy * 2.0f - 1.0f;
	float2 z  = mul(float4(0.0f, 0.0f, float(index.z) / (dimension.z - 1), 1.0f), gCamera.proj).zw;
	
	float4 view   = mul(float4(d.x, -d.y, z.x / z.y, 1.0f), gCamera.projInv);
	view /= view.w;

	float3 position = mul(view, gCamera.world).xyz;

	float3 p_light = gTransform.GetPosition();
	float r        = length(p_light - position); //!< lightとsurfaceの距離
	float3 c_light = gPointLight.GetColor(r);

	gVoxel[index] = float4(c_light, 1.0f);
	
}
