#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// DeferredBufferIndex structure
////////////////////////////////////////////////////////////////////////////////////////////
struct DeferredBufferIndex {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint albedo;
	uint normal;
	uint materialARM;
	uint position;
	uint velocity;
	uint depth;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float GetDepth(uint2 index) {
		Texture2D<float> texture = ResourceDescriptorHeap[depth];
		return texture.Load(uint3(index, 0));
	}

	float3 GetAlbedo(uint2 index) {
		Texture2D<float4> texture = ResourceDescriptorHeap[albedo];
		return texture.Load(uint3(index, 0)).rgb;
	}

	float3 GetNormal(uint2 index) {
		Texture2D<float4> texture = ResourceDescriptorHeap[normal];
		return normalize(texture.Load(uint3(index, 0)).rgb * 2.0f - 1.0f);
	}

	float3 GetMaterial(uint2 index) {
		Texture2D<float4> texture = ResourceDescriptorHeap[materialARM];
		return texture.Load(uint3(index, 0)).rgb;
	}

	float3 GetPosition(uint2 index) {
		Texture2D<float4> texture = ResourceDescriptorHeap[position];
		return texture.Load(uint3(index, 0)).rgb;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// DeferredBufferIndexConstantBuffer structure
////////////////////////////////////////////////////////////////////////////////////////////
struct DeferredBufferIndexConstantBuffer {
	
	//=========================================================================================
	// public variables
	//=========================================================================================
	
	DeferredBufferIndex index;

	//=========================================================================================
	// public methods
	//=========================================================================================

	DeferredBufferIndex Get() {
		return index;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// DeferredSurface structure
////////////////////////////////////////////////////////////////////////////////////////////
struct DeferredSurface {
	
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

	bool GetSurface(DeferredBufferIndex parameter, uint2 index) {
		depth = parameter.GetDepth(index);
		
		if (depth == 1.0f) {
#ifndef _COMPUTE
			discard; //!< object is not exist.
#endif
			return false;
		}
		
		position = parameter.GetPosition(index);
		albedo   = parameter.GetAlbedo(index);
		normal   = parameter.GetNormal(index);

		float3 material = parameter.GetMaterial(index);
		ao        = material.r;
		roughness = material.g;
		metallic  = material.b;

		return true;
	}
	
};
