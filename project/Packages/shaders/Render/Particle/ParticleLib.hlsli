#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleLib namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ParticleLib {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Albedo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Albedo {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint type;
		//!< 0: albedo, 1: texture mul albedo

		float3 albedo;
		uint index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float3 GetAlbedo(SamplerState sample, float2 texcoord) {
			switch (type) {
				case 1:{
						Texture2D<float4> texture = ResourceDescriptorHeap[index];
						return texture.Sample(sample, texcoord).rgb * albedo;
					}
					break;

				default:
					return albedo;
					break;
			}
		}
	};
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// Transparent structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transparent {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint type;
		//!< 0: transparent, 1: texture mul transparent

		float transparent;
		uint index;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float GetTransparent(SamplerState sample, float2 texcoord) {
			switch (type) {
				case 1:{
						Texture2D<float4> texture = ResourceDescriptorHeap[index];
						return texture.Sample(sample, texcoord).a * transparent;
					}
					break;

				default:
					return transparent;
					break;
			}
		}
		
	};
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleConfig structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ParticleConfig {

	//=========================================================================================
	// public variables
	//=========================================================================================

	ParticleLib::Albedo albedo;
	ParticleLib::Transparent transparent;

	//* flags *//

	uint isBillboard;
	
};
