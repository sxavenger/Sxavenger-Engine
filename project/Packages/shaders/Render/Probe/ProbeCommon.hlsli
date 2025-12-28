#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ProbeReservoir.hlsli"

//* library
#include "../../Library/Octahedral.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

#define NUM_THREAD_X 16
#define NUM_THREAD_Y 16

////////////////////////////////////////////////////////////////////////////////////////////
// ProbeConfig structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ProbeConfig {
	uint3 probeCount;
	uint sampleCount;
	uint2 probeResolution;
	float probeOffset;
	float hysteresis;
};

////////////////////////////////////////////////////////////////////////////////////////////
// ProbeCage structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ProbeCage {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint3 probeIndices[8];

	//=========================================================================================
	// public methods
	//=========================================================================================

	static bool GetCage(ProbeConfig config, float3 surface_position, float3 base_position, out ProbeCage cage) {

		cage = (ProbeCage)0;

		float3 relative_position = surface_position - floor(base_position);

		float3 range_min = -float3(config.probeCount) / 2.0f * config.probeOffset;
		float3 range_max =  float3(config.probeCount) / 2.0f * config.probeOffset;

		if (any(relative_position < range_min) || any(relative_position > range_max)) {
			return false; //!< out of range
		}

		float3 grid_pos = relative_position / config.probeOffset + float3(config.probeCount) / 2.0f;
		uint3 base_index = (uint3)floor(grid_pos);

		// 8 頂点（2x2x2）
		uint idx = 0;
		[unroll]
		for (uint z = 0; z < 2; ++z) {
			[unroll]
			for (uint y = 0; y < 2; ++y) {
				[unroll]
				for (uint x = 0; x < 2; ++x) {
					uint3 probe_index = base_index + uint3(x, y, z);

					// グリッド境界チェック
					if (any(probe_index >= config.probeCount)) {
						return false;
					}

					cage.probeIndices[idx++] = probe_index;
				}
			}
		}

		return true;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 GetProbeDirection(float2 texcoord) {
	return DecodeOctahedral(texcoord);
}

float2 GetProbeTexcoord(float3 direction) {
	return EncodeOctahedral(direction);
}

float3 GetProbePosition(uint3 probeIndex, ProbeConfig config, float3 position) {
	float3 offset_count = float3(probeIndex) - float3(config.probeCount) / 2.0f;
	float3 probe_position = floor(position) + offset_count * config.probeOffset;
	//!< FIXME: position calculation.

	return probe_position;
}
