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
	
	float luminance_threshold_x;
	float luminance_x_lit; //!< [0 ~ 1]の範囲で指定する.
	
	float luminance_threshold_y;
	float luminance_y_lit; //!< [0 ~ 1]の範囲で指定する.

	float hatch_rotation_min;
	float hatch_rotation_max;

	float hatch_noise_threshold;
	float hatch_noise_power;
	
	float hatch_luminance_threshold;
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

uint GetColorLevel(float luminance) {
	if (luminance > gParameter.luminance_threshold_x) {
		return 0;
		
	} else if (luminance > gParameter.luminance_threshold_y) {
		return 1;
		
	} else {
		return 2;
	}
}

float2 DirectionToSphericalTexcoord(float3 direction) {
	direction = normalize(direction);
	
	float phi = atan2(direction.z, direction.x); // [-π, π]
	float theta = acos(clamp(direction.y, -1.0, 1.0)); // [0, π]

	float2 uv;
	uv.x = phi * (1.0 / Mathmatic::kTau) + 0.5; // [0, 1]
	uv.y = theta * (1.0 / Mathmatic::kPi); // [0, 1]

	return uv;
}

float GetHatchNoise(float3 position, float3 normal) {
	//!< 法線からハッチの回転量の取得
	float2 rotation_noise_texcoord = DirectionToSphericalTexcoord(normal);
	float2 rotation_noise_uv       = floor(rotation_noise_texcoord / 10.0f) * 10.0f;

	float rotation_noise = gNoiseTexture.SampleLevel(gWarpSampler, rotation_noise_uv, 0).r;
	float rotation       = lerp(gParameter.hatch_rotation_min, gParameter.hatch_rotation_max, rotation_noise);

	float hatch_noise = 1.0f;

	//!< ハッチノイズの回転とスケールの行列の生成
	float4x4 hatch_matrix = mul(Matrix2d::MakeRotate(rotation), Matrix2d::MakeScale(gParameter.noise_scale));
	
		//!< ハッチノイズの取得
	float2 hatch_noise_texcoord = (position.xy + position.z) * rcp( /*gParameter.position_frequency =*/100.0); //!< 3d座標からnoiseのサンプルpointの生成
	float2 hatch_noise_uv = mul(float4(hatch_noise_texcoord, 0.0f, 1.0f), hatch_matrix).xy;

	hatch_noise *= gNoiseTexture.SampleLevel(gWarpSampler, hatch_noise_uv, 0).r;
	
	return hatch_noise;
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
	
	uint level = GetColorLevel(luminance);

	float3 output = float3(0.0f, 0.0f, 0.0f);

	switch (level) {
		case 0:
			output = color;
			break;
		
		case 1:
			{
				output = surface.albedo * gParameter.luminance_x_lit;
			}
			break;
		
		case 2:
			{
				output = surface.albedo * gParameter.luminance_y_lit;

				float hatch_noise = GetHatchNoise(surface.position, surface.normal);

				if (hatch_noise < gParameter.hatch_noise_threshold) {
					hatch_noise = saturate(hatch_noise / gParameter.hatch_noise_threshold); //!< threasholdを基準とした[0, 1]に正規化
					hatch_noise = pow(hatch_noise, gParameter.hatch_noise_power);

				} else {
					hatch_noise = 1.0f; //!< hatchの無効化.
				}
	
				//!< luminanceの調整
				float hatch_luminance = 1.0f;
			
				if (luminance < gParameter.hatch_luminance_threshold) {
					hatch_luminance = saturate(luminance / gParameter.hatch_luminance_threshold); //!< threasholdを基準とした[0, 1]に正規化
				}

				float t = lerp(hatch_noise, 1.0f, hatch_luminance);
				t = floor(t * 10.0f) * 0.1f; //!< [0, 1]の範囲で10段階に分割する.
				
				output = lerp(output * 0.2f, output, t);
				
			}
			break;
		
		default:
			output = float3(1.0, 0.0, 1.0);
	}

	gOutput[index] = float4(output, transparency);
	
}
