//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Process.hlsli"
#include "../../../Component/CameraComponent.hlsli"
#include "../../../Component/TransformComponent.hlsli"
#include "../../../Component/LightComponentCommon.hlsli"
#include "../../../Component/DirectionalLightComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* output
RWTexture2D<float4> gOutput : register(u0);

//* texture
//Texture3D<float> gVolumetric : register(t0);
Texture2D<float> gDepth      : register(t1);

//* scene
RaytracingAccelerationStructure gScene  : register(t2);
ConstantBuffer<CameraComponent> gCamera : register(b0);

//* light
struct LightCount {
	uint count;
};

//# directional light
ConstantBuffer<LightCount>                  gDirectionalLightCount      : register(b1);
StructuredBuffer<TransformComponent>        gDirectionalLightTransforms : register(t3);
StructuredBuffer<DirectionalLightComponent> gDirectionalLightParams     : register(t4);
StructuredBuffer<InlineShadow>              gDirectionalLightShadows    : register(t5);

static const float kTMin = 0.001f;
static const float kTMax = 10000.0f;

//* parameter
struct Parameter {
	uint step;
};
ConstantBuffer<Parameter> gParameter : register(b2);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float Hash(float3 p) {
	return frac(sin(dot(p, float3(12.9898, 78.233, 45.164))) * 43758.5453);
}

float FixLinearDepth(float depth) {
	return depth / (gCamera.far / gCamera.near - depth * (gCamera.far / gCamera.near - 1.0f));
}

float InverseFixLinearDepth(float linearDepth) {
	float f = gCamera.far / gCamera.near;
	return (linearDepth * f) / (1.0f + linearDepth * (f - 1.0f));
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispathThreadId : SV_DispatchThreadID) {

	uint2 index = dispathThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return;
	}

	float depth = FixLinearDepth(gDepth.Load(uint3(index, 0)));
	float2 d    = (index.xy + 0.5f) / size * 2.0f - 1.0f;

	float3 color = float3(0.0f, 0.0f, 0.0f);

	for (uint i = 0; i <= gParameter.step; ++i) {

		float t = (float)i / (float)gParameter.step;

		//if (t >= depth) {
		//	break;
		//}

		float4 target = mul(float4(d.x, -d.y, InverseFixLinearDepth(t), 1.0f), gCamera.projInv);
		target /= target.w;

		float4 position = mul(target, gCamera.world);
		position /= position.w;
		
		float weight = Hash(float3(d, i));

		for (uint j = 0; j < gDirectionalLightCount.count; ++j) {

			//* Lightの情報を取得
			float3 c_light = gDirectionalLightParams[j].GetColor(); //!< lightのcolor
			float3 l       = -gDirectionalLightTransforms[j].GetDirection(); //!< surfaceからlightへの方向ベクトル

			//* shadow
			RayDesc desc;
			desc.Origin    = position.xyz;
			desc.Direction = l;
			desc.TMin      = kTMin;
			desc.TMax      = kTMax;

			c_light *= gDirectionalLightShadows[j].TraceShadow(desc, gScene);

			color += saturate(c_light) * weight * (1.0f / gParameter.step);
		}
	}

	gOutput[index].rgb += color;
	
};
