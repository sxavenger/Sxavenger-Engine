#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// SkyAtmosphereFlag namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SkyAtmosphereFlag {
	static const uint None        = 0,
	                  Environment = 1 << 0; //!< Use environment map
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkyAtmosphereComponent structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SkyAtmosphereComponent {
	
	//=========================================================================================
	// public variables
	//=========================================================================================

	uint flags;

	uint index;
	
	//=========================================================================================
	// public methods
	//=========================================================================================

	float4 GetEnvironment(SamplerState sample, float3 direction) {
		if (!(flags & SkyAtmosphereFlag::Environment)) {
			return float4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		TextureCube<float4> environment = ResourceDescriptorHeap[index];
		
		float4 color = environment.SampleLevel(sample, direction, 0);
		return color;
	
	}
	
};
