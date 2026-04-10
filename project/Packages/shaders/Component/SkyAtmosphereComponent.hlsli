#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../Library/Octahedral.hlsli"
#include "../Library/Mathmatic.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// SkyAtmosphereFlag namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SkyAtmosphereFlag {
	static const uint None        = 0,
	                  Environment = 1 << 0, //!< Use environment map
	                  Aerial      = 1 << 1; //!< Use aerial perspective
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkyAtmosphereComponent structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SkyAtmosphereComponent {
	
	//=========================================================================================
	// public variables
	//=========================================================================================

	uint flags;

	uint environmentIndex;
	uint aerialIndex;
	
	//=========================================================================================
	// public methods
	//=========================================================================================

	float4 GetEnvironment(SamplerState sample, float3 direction) {
		
		if (!(flags & SkyAtmosphereFlag::Environment)) {
			return float4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		TextureCube<float4> environment = ResourceDescriptorHeap[environmentIndex];
		
		float4 color = environment.SampleLevel(sample, direction, 0);
		return color;
	
	}

	float4 GetAerial(SamplerState sample, float3 direction, float distance) {
		
		if (!(flags & SkyAtmosphereFlag::Aerial)) {
			return float4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		//distance /= 100.0f; //!< Convert to kilometer.
		
		Texture3D<float4> aerial = ResourceDescriptorHeap[aerialIndex];

		static const float kApDistancePerSlice = 100.0f; //!< Aerial.cs.hlsl: slice distance.
		
		float2 octahedral = Octahedral::EncodeOctahedral(direction);
		
		float slice = distance / kApDistancePerSlice;
		float s     = saturate(slice / 16.0f);
		float u = sqrt(s);

		// index
		float index = u * 16.0f;

		return aerial.SampleLevel(sample, float3(octahedral, index), 0);
	}
	
};
