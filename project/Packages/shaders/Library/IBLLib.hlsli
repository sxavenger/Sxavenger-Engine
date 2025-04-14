#pragma once

/*
# reference

*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Math.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// defalut values
////////////////////////////////////////////////////////////////////////////////////////////
SamplerState kDefaultSampler;

////////////////////////////////////////////////////////////////////////////////////////////
// IBLParameter structure
////////////////////////////////////////////////////////////////////////////////////////////
struct IBLParameter {
	//!< image based lighting parameter.

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint type;
	//!< 0: none, 1: value, 2: texture cube map, 3: texture 2d
	
	uint index;
	float4 color;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float4 GetColor(float3 direction, SamplerState sample = kDefaultSampler) {
		switch (type) {
			case 1:
				{
					return color;
				}
				break;
				

			case 2:
				{
					TextureCube<float4> texture = ResourceDescriptorHeap[index];
					return texture.Sample(sample, direction);
				}
				break;

			case 3:
				{
					Texture2D<float4> texture = ResourceDescriptorHeap[index];
					float theta = acos(clamp(direction.y, -1.0f, 1.0f));
					float p = atan2(direction.z, direction.x);
					float phi = (p < 0.0f) ? p + kTau : p;

					float2 uv = float2(saturate(phi * kTauInv), saturate(theta * kPiInv));
					return texture.Sample(sample, uv);
				}
				break;
		}

		return float4(0, 0, 0, 0);
	}
	
};

