#pragma once

/*
# reference

*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////
// IBLParameter structure
////////////////////////////////////////////////////////////////////////////////////////////
struct IBLParameter {
	//!< image based lighting parameter.

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint type;
	//!< 0: none, 1: value, 2: texture cube map
	
	float3 color;
	uint index;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float3 Get(SamplerState sample, float3 refection) {
		switch (type) {
			case 0:
				{
					return float3(0.0f, 0.0f, 0.0f);
				}
				break;

			case 1:
				{
					return color;
				}
				break;
				

			case 2:
				{
					TextureCube<float4> texture = ResourceDescriptorHeap[index];
					return texture.Sample(sample, refection).rgb;
				}
				break;
		}
	}
	
};

