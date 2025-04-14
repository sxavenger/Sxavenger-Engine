#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------


//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float> gDepth     : register(t10);
Texture2D<float4> gAlbedo   : register(t11);
Texture2D<float4> gNormal   : register(t12);
Texture2D<float4> gPosition : register(t13);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float GetDepth(uint2 index) {
	return gDepth.Load(uint3(index, 0));
}

float3 GetAlbedo(uint2 index) {
	return gAlbedo.Load(uint3(index, 0)).rgb;
}

float3 GetNormal(uint2 index) {
	float3 normal = gNormal.Load(uint3(index, 0)).rgb;
	return normalize(normal * 2.0f - 1.0f);
}

float3 GetPosition(uint2 index) {
	return gPosition.Load(uint3(index, 0)).rgb;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Surface structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Surface {

	//* member *//
	
	float3 position;
	float3 albedo;
	float3 normal;
	float  depth;
	
	//* methods *//
	
	bool GetSurface(uint2 index) {

		depth = GetDepth(index);
		
		if (depth == 1.0f) {
			return false; //!< object is not exist.
		}
		
		position = GetPosition(index);
		albedo = GetAlbedo(index);
		normal = GetNormal(index);
		
		return true;
	}
	
};


