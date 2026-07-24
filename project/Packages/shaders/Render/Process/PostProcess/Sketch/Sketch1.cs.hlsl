//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../PostProcess.hlsli"
#include "../../../../Library/ACES.hlsli"
#include "../../../../Library/RandomLib.hlsli"
#include "../../../../Library/Octahedral.hlsli"
#include "../../../../Component/CameraComponent.hlsli"
#include "../../../GBuffer.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

//* textures
Texture2D<float4> gInput : register(t0);
RWTexture2D<float4> gOutput : register(u0);

struct Parameter {
	float2 noise_scale;
	float noise_threshold;
	float noise_power;
	
	float luminance_threshold;
	
	float rotation_range_min;
	float rotation_range_max;

	float position_frequency;
};
ConstantBuffer<Parameter> gParameter : register(b0);

ConstantBuffer<CameraComponent> gCamera : register(b1);

Texture2D<float> gNoiseTexture : register(t1);
SamplerState gWarpSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

namespace Matrix2d {

	float4x4 MakeScale(float2 v) {
		return float4x4(
			v.x, 0.0f, 0.0f, 0.0f,
			0.0f, v.y, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
	
	float4x4 MakeRotate(float v) {
		return float4x4(
			cos(v), sin(v), 0.0f, 0.0f,
			-sin(v), cos(v), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
	
}

float2 DirectionToSphericalTexcoord(float3 direction) {
	direction = normalize(direction);
	
	float phi   = atan2(direction.z, direction.x); // [-π, π]
	float theta = acos(clamp(direction.y, -1.0, 1.0)); // [0, π]

	float2 uv;
	uv.x = phi * (1.0 / Mathmatic::kTau) + 0.5; // [0, 1]
	uv.y = theta * (1.0 / Mathmatic::kPi);      // [0, 1]

	return uv;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void main(uint3 dispathThreadId : SV_DispatchThreadID) {

	uint2 index = dispathThreadId.xy;

	if (CheckOverDimension(index.xy)) {
		return;
	}

	GBuffer::FetchArgument argument = GBuffer::FetchArgument::Create(index, dimension, gCamera.projInv, gCamera.world);
	GBuffer::Surface surface;
	if (!surface.FetchSurface(argument)) {
		gOutput[index] = gInput[index];
		return; //!< surfaceが見つからない.
	}

	float3 color       = gInput[index].rgb;
	float transparency = gInput[index].a;

	float luminance = dot(color, ACES::AP1_RGB2Y);
	
	//!< 法線からハッチの回転量の取得
	float2 rotation_noise_texcoord = DirectionToSphericalTexcoord(surface.normal);
	float2 rotation_noise_uv       = floor(rotation_noise_texcoord / 10.0f) * 10.0f;

	float rotation_noise = gNoiseTexture.SampleLevel(gWarpSampler, rotation_noise_uv, 0).r;
	float rotation       = lerp(gParameter.rotation_range_min, gParameter.rotation_range_max, rotation_noise);

	//!< ハッチノイズの回転とスケールの行列の生成
	float4x4 hatch_matrix = mul(Matrix2d::MakeRotate(rotation), Matrix2d::MakeScale(gParameter.noise_scale));
	
	//!< ハッチノイズの取得
	float2 hatch_noise_texcoord = (surface.position.xy + surface.position.z) * rcp(gParameter.position_frequency); //!< 3d座標からnoiseのサンプルpointの生成
	float2 hatch_noise_uv       = mul(float4(hatch_noise_texcoord, 0.0f, 1.0f), hatch_matrix).xy;
	
	float hatch_noise = gNoiseTexture.SampleLevel(gWarpSampler, hatch_noise_uv, 0).r;
	
	//!< ハッチノイズの調整
	if (hatch_noise < gParameter.noise_threshold) {
		hatch_noise = hatch_noise / gParameter.noise_threshold; //!< threasholdを基準とした[0, 1]に正規化
		hatch_noise = pow(hatch_noise, gParameter.noise_power);

	} else {
		hatch_noise = 1.0f; //!< hatchの無効化.
	}
	
	//!< luminanceの調整
	if (luminance < gParameter.luminance_threshold) {
		luminance = luminance / gParameter.luminance_threshold; //!< threasholdを基準とした[0, 1]に正規化

	} else {
		luminance = 1.0f; //!< luminanceの無効化.
	}
	
	float3 output = color * lerp(hatch_noise, 1.0f, saturate(luminance));
	
	gOutput[index] = float4(output, transparency);
	
}
