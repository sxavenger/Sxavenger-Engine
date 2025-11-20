#pragma once

#ifndef RANDOM_LIBRARY
#define RANDOM_LIBRARY

////////////////////////////////////////////////////////////////////////////////////////////
// WhiteNoise namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace WhiteNoise {

	/*
	# reference
	 - https://www.ronja-tutorials.com/post/024-white-noise/
	*/

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

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
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// Xorshift namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Xorshift {

	/*
	# reference
	 - https://ja.wikipedia.org/wiki/Xorshift
	*/

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	uint xorshift32(uint x) {
		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;
		return x;
	}

	uint2 xorshift32(uint2 x) {
		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;
		return x;
	}

	uint3 xorshift32(uint3 x) {
		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;
		return x;
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// pseudo random methods
////////////////////////////////////////////////////////////////////////////////////////////

float PseudoRandom(float2 xy) {
	float2 v = frac(xy * rcp(128.0f)) * 128.0f + float2(-64.340622, -72.898437);
	return frac(dot(v.xyx * v.xyy, float3(20.390625, 33.923847, 4.232142)));
}

#endif
