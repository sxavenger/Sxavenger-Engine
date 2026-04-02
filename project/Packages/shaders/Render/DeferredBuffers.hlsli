#pragma once

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float>  gDepth    : register(t0, space3);
Texture2D<float4> gAlbedo   : register(t1, space3);
Texture2D<float4> gNormal   : register(t2, space3);
Texture2D<float4> gPosition : register(t3, space3);
Texture2D<float4> gMaterial : register(t4, space3);

////////////////////////////////////////////////////////////////////////////////////////////
// Surface structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Surface {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float3 position;
	float3 albedo;
	float3 normal;

	float depth;

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

	static float3 GetNormal(uint2 pixel) {
		float3 normal = gNormal.Load(uint3(pixel, 0)).rgb;
		return normalize(normal * 2.0f - 1.0f);
	}

	static float3 GetPosition(uint2 pixel) {
		return gPosition.Load(uint3(pixel, 0)).rgb;
	}

	static float3 GetMaterial(uint2 pixel) {
		return gMaterial.Load(uint3(pixel, 0)).rgb;
	}
	
	//* main methods *//
	
	bool GetSurface(uint2 index) {

		depth = Surface::GetDepth(index);
		
		if (depth == 1.0f) {
			//!< object is not exist.
#ifndef _COMPUTE_SHADER
			discard;
#endif
			return false;
		}
		
		position = Surface::GetPosition(index);
		albedo   = Surface::GetAlbedo(index);
		normal   = Surface::GetNormal(index);

		float3 material = Surface::GetMaterial(index);
		ao        = material.r;
		roughness = material.g;
		metallic  = material.b;

		return true;
	}
	
};

