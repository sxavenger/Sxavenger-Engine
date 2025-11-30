#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightFlag namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace SkyLightFlag {
	static const uint None        = 0,
	                  Environment = 1 << 0, //!< Use environment map
	                  Irradiance  = 1 << 1, //!< Use irradiance map
	                  Radiance    = 1 << 2; //!< Use radiance map

}

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightComponent structure
////////////////////////////////////////////////////////////////////////////////////////////
struct SkyLightComponent {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint flags;

	uint environmentIndex;
	uint irradianceIndex;
	uint radianceIndex;

	float intensity;
	uint  radianceMipmaps;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float4 GetEnvironment(SamplerState sample, float3 direction) {
		if (!(flags & SkyLightFlag::Environment)) {
			return float4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		TextureCube<float4> environment = ResourceDescriptorHeap[environmentIndex];
		
		float4 color = environment.SampleLevel(sample, direction, 0);
		color.rgb *= intensity;
		return color;
	}

	float4 GetIrradiance(SamplerState sample, float3 direction) {
		if (!(flags & SkyLightFlag::Irradiance)) {
			return float4(0.0f, 0.0f, 0.0f, 0.0f);
		}
		
		TextureCube<float4> irradiance = ResourceDescriptorHeap[irradianceIndex];

		float4 color = irradiance.SampleLevel(sample, direction, 0);
		color.rgb *= intensity;
		return color;
	}

	float4 GetRadiance(SamplerState sample, float3 direction, float roughness) {
		if (!(flags & SkyLightFlag::Radiance)) {
			return float4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		float lod = roughness * float(radianceMipmaps);
		TextureCube<float4> radiance = ResourceDescriptorHeap[irradianceIndex];
		
		float4 color = radiance.SampleLevel(sample, direction, lod);
		color.rgb *= intensity;
		return color;
	}
	
};
