#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../../Library/Octahedral.hlsli"
#include "../../Library/ImportanceSample.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Lux namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Lux {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Setting structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Setting {

		//=========================================================================================
		// public variables
		//=========================================================================================
		
		uint2 downscale;
		uint2 atlasDimension;
		uint sampleStepCount;

		//=========================================================================================
		// public methods
		//=========================================================================================

		uint2 GetDownscalePixel(uint2 pixel) {
			return pixel / downscale;
		}
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Moment structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Moment {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint sampleCount;
		uint jitter;

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Update(Setting setting) {
			sampleCount += setting.sampleStepCount;
			sampleCount = sampleCount % (setting.atlasDimension.x * setting.atlasDimension.y);
		}

		uint GetAtlasIndex(Setting setting, uint sampleIndex) {
			return (sampleCount + sampleIndex + jitter) % (setting.atlasDimension.x * setting.atlasDimension.y);
		}

		uint GetAtlasIndex(Setting setting, uint2 atlas_pixel) {
			return atlas_pixel.y * setting.atlasDimension.x + atlas_pixel.x;
		}

		uint2 GetAtlasPixel(Setting setting, uint sampleIndex) {
			uint atlasIndex = GetAtlasIndex(setting, sampleIndex);
			return uint2(atlasIndex % setting.atlasDimension.x, atlasIndex / setting.atlasDimension.x);
		}

		static Moment GetMoment(uint2 value) {
			Moment moment;
			moment.sampleCount = value.x;
			moment.jitter      = value.y;
			return moment;
		}

		uint2 GetValue() {
			return uint2(sampleCount, jitter);
		}

		static uint GetJitter();
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	float3 ConvertDirection(float3 direction, float3 normal) {
		float3 tangent   = normalize(cross(abs(normal.y) < 0.999f ? float3(0, 1, 0) : float3(1, 0, 0), normal));
		float3 bitangent = cross(normal, tangent);

		return direction.x * tangent + direction.y * normal + direction.z * bitangent;
	}

	uint2 GetSurfacePixel(uint2 pixel, Setting setting) {
		uint2 downscaled = pixel / setting.atlasDimension;
		return downscaled * setting.downscale;
	}
	
}
