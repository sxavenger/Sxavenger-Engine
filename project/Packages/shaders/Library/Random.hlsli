#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// WhiteNoise namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace WhiteNoise {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	/*
	# reference
	 - https://www.ronja-tutorials.com/post/024-white-noise/
	*/

	float Rand3dTo1d(float3 value, float3 dotDir = float3(12.9898, 78.233, 37.719)) {
		//make value smaller to avoid artefacts
		float3 smallValue = sin(value);
		//get scalar value from 3d vector
		float random = dot(smallValue, dotDir);
		//make value more random by making it bigger and then taking the factional part
		random = frac(sin(random) * 143758.5453);
		return random;
	}

	float Rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233)) {
		float2 smallValue = sin(value);
		float random = dot(smallValue, dotDir);
		random = frac(sin(random) * 143758.5453);
		return random;
	}

	float Rand1dTo1d(float3 value, float mutator = 0.546) {
		float3 random = frac(sin(value + mutator) * 143758.5453);
		return random.x;
	}

	//to 2d functions

	float2 Rand3dTo2d(float3 value) {
		return float2(
			Rand3dTo1d(value, float3(12.989, 78.233, 37.719)),
			Rand3dTo1d(value, float3(39.346, 11.135, 83.155))
		);
	}

	float2 Rand2dTo2d(float2 value) {
		return float2(
			Rand2dTo1d(value, float2(12.989, 78.233)),
			Rand2dTo1d(value, float2(39.346, 11.135))
		);
	}

	float2 Rand1dTo2d(float value) {
		return float2(
			Rand2dTo1d(value, 3.9812),
			Rand2dTo1d(value, 7.1536)
		);
	}

	//to 3d functions

	float3 Rand3dTo3d(float3 value) {
		return float3(
			Rand3dTo1d(value, float3(12.989, 78.233, 37.719)),
			Rand3dTo1d(value, float3(39.346, 11.135, 83.155)),
			Rand3dTo1d(value, float3(73.156, 52.235, 09.151))
		);
	}

	float3 Rand2dTo3d(float2 value) {
		return float3(
			Rand2dTo1d(value, float2(12.989, 78.233)),
			Rand2dTo1d(value, float2(39.346, 11.135)),
			Rand2dTo1d(value, float2(73.156, 52.235))
		);
	}

	float3 Rand1dTo3d(float value) {
		return float3(
			Rand1dTo1d(value, 3.9812),
			Rand1dTo1d(value, 7.1536),
			Rand1dTo1d(value, 5.7241)
		);
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// Random structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Random {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float3 seed;

		//=========================================================================================
		// public methods
		//=========================================================================================

		float3 Random3d() {
			seed = Rand3dTo3d(seed);
			return seed;
		}

		float Random1d() {
			seed.x = Rand3dTo1d(seed);
			return seed.x;
		}
		
	};
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// Xorshift namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Xorshift {

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	uint xorshift32(uint x) {
		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;
		return x;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// Random structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Random {

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint seed;

		//=========================================================================================
		// public methods
		//=========================================================================================

		uint Random1d() {
			seed = xorshift32(seed);
			return seed;
		}
		
	};
	
}
