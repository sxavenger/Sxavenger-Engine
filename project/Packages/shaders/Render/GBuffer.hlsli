#pragma once

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float>  gDepth       : register(t0, space3);
Texture2D<float4> gAlbedo      : register(t1, space3);
Texture2D<float4> gNormal      : register(t2, space3);
Texture2D<float4> gMaterialARM : register(t3, space3);

////////////////////////////////////////////////////////////////////////////////////////////
// GBuffer namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace GBuffer {
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// FetchArgument structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct FetchArgument {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint2 pixel;
		uint2 resolution;

		float4x4 mat; //!< projInv * viewInv

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* helper methods *//

		static uint2 GetResolution() {
			uint2 resolution;
			gDepth.GetDimensions(resolution.x, resolution.y);
			return resolution;
		}

		//* constructor methods *//
		
		static FetchArgument Create(uint2 pixel, uint2 resolution, float4x4 projInv, float4x4 viewInv) {
			FetchArgument argument;
			argument.pixel      = pixel;
			argument.resolution = resolution;
			argument.mat        = mul(projInv, viewInv);

			return argument;
		}

		static FetchArgument Create(uint2 pixel, float4x4 projInv, float4x4 viewInv) {
			FetchArgument argument;
			argument.pixel      = pixel;
			argument.resolution = GetResolution();
			argument.mat        = mul(projInv, viewInv);

			return argument;
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SampleArgument structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SampleArgment {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float2 uv;
		SamplerState pointSampler;

		float4x4 mat; //!< projInv * viewInv

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor methods *//

		static SampleArgment Create(float2 uv, SamplerState pointSampler, uint2 resolution, float4x4 projInv, float4x4 viewInv) {
			SampleArgment argument;
			argument.uv           = uv;
			argument.pointSampler = pointSampler;
			argument.mat          = mul(projInv, viewInv);

			return argument;
		}

		static SampleArgment Create(float2 uv, SamplerState pointSampler, float4x4 projInv, float4x4 viewInv) {
			SampleArgment argument;
			argument.uv           = uv;
			argument.pointSampler = pointSampler;
			argument.mat          = mul(projInv, viewInv);

			return argument;
		}
		
	};
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// Surface structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Surface {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float depth;

		float3 albedo;
		float3 normal;
		float3 position;

		float ao;
		float roughness;
		float metallic;

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* helper methods *//

		static float GetDepth(uint2 pixel) {
			return gDepth.Load(uint3(pixel, 0));
		}

		static float GetDepth(float2 uv, SamplerState pointSampler) {
			return gDepth.SampleLevel(pointSampler, uv, 0).r;
		}

		static float3 GetAlbedo(uint2 pixel) {
			return gAlbedo.Load(uint3(pixel, 0)).rgb;
		}

		static float3 GetAlbedo(float2 uv, SamplerState pointSampler) {
			return gAlbedo.SampleLevel(pointSampler, uv, 0).rgb;
		}

		static float3 GetNormal(uint2 pixel) {
			float3 normal = gNormal.Load(uint3(pixel, 0)).rgb;
			return normalize(normal * 2.0f - 1.0f); //!< [-1, 1]
		}

		static float3 GetNormal(float2 uv, SamplerState pointSampler) {
			float3 normal = gNormal.SampleLevel(pointSampler, uv, 0).rgb;
			return normalize(normal * 2.0f - 1.0f); //!< [-1, 1]
		}

		static float3 GetPosition(uint2 pixel, uint2 resolution, float4x4 mat /* = projectionInv * viewInv */) {

			float depth = GetDepth(pixel);

			float2 ndc = float2(pixel) / float2(resolution) * 2.0f - 1.0f; //!< [-1, 1]
			ndc.y *= -1.0f; //!< [1, -1]

			float4 clip = float4(ndc, depth, 1.0f);
			
			float4 world = mul(clip, mat);
			return world.xyz / world.w;
		}

		static float3 GetPosition(float2 uv, SamplerState pointSampler, float4x4 mat /* = projectionInv * viewInv */) {

			float depth = GetDepth(uv, pointSampler);

			float2 ndc = uv * 2.0f - 1.0f; //!< [-1, 1]
			ndc.y *= -1.0f; //!< [1, -1]

			float4 clip = float4(ndc, depth, 1.0f);
			
			float4 world = mul(clip, mat);
			return world.xyz / world.w;
		}

		static float3 GetMaterial(uint2 pixel) {
			return gMaterialARM.Load(uint3(pixel, 0)).rgb;
		}

		static float3 GetMaterial(float2 uv, SamplerState pointSampler) {
			return gMaterialARM.SampleLevel(pointSampler, uv, 0).rgb;
		}

		//* main methods *//

		bool FetchSurface(FetchArgument argument) {

			depth = Surface::GetDepth(argument.pixel);

			if (depth == 1.0f) {
				//!< object is not exist.
#ifndef _COMPUTE_SHADER
				discard;
#endif
				return false;
			}

			albedo   = Surface::GetAlbedo(argument.pixel);
			normal   = Surface::GetNormal(argument.pixel);
			position = Surface::GetPosition(argument.pixel, argument.resolution, argument.mat);

			float3 material = Surface::GetMaterial(argument.pixel);
			ao        = material.r;
			roughness = material.g;
			metallic  = material.b;

			return true;
		}

		bool SampleSurface(SampleArgment argument) {

			depth = Surface::GetDepth(argument.uv, argument.pointSampler);

			if (depth == 1.0f) {
				//!< object is not exist.
				return false;
			}

			albedo   = Surface::GetAlbedo(argument.uv, argument.pointSampler);
			normal   = Surface::GetNormal(argument.uv, argument.pointSampler);
			position = Surface::GetPosition(argument.uv, argument.pointSampler, argument.mat);

			float3 material = Surface::GetMaterial(argument.uv, argument.pointSampler);
			ao        = material.r;
			roughness = material.g;
			metallic  = material.b;

			return true;
		}
		
	};
	
}
