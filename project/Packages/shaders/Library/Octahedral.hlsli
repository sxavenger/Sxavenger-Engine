#pragma once

/*
 # reference
 - [Octahedral Mapping](https://gamedev.stackexchange.com/questions/169508/octahedral-impostors-octahedral-mapping?answertab=scoredesc#tab-top)
*/

#ifndef OCTAHEDRAL
#define OCTAHEDRAL

////////////////////////////////////////////////////////////////////////////////////////////
// Octahedral namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Octahedral {

	////////////////////////////////////////////////////////////////////////////////////////////
	// [Octahedral - 3d] methods
	////////////////////////////////////////////////////////////////////////////////////////////

	float2 EncodeOctahedral(float3 direction) {

		float3 octant = sign(direction);

		float sum = dot(direction, octant);
		float3 octahedron = direction / sum;

		if (octahedron.z < 0) {
			float3 absolute = abs(octahedron);
			octahedron.xy = octant.xy * float2(1.0f - absolute.y, 1.0f - absolute.x);
		}

		return octahedron.xy * 0.5f + 0.5f;
	}

	float3 DecodeOctahedral(float2 encoded) {

		float3 position = float3(2.0f * (encoded - 0.5f), 0);

		float2 absolute = abs(position.xy);
		position.z = 1.0f - absolute.x - absolute.y;

		if (position.z < 0) {
			position.xy = sign(position.xy) * float2(1.0f - absolute.y, 1.0f - absolute.x);
		}

		return position;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// [Pyramid - Yup] methods
	////////////////////////////////////////////////////////////////////////////////////////////

	float2 EncodePyramid(float3 direction) {

		float3 octant = sign(direction);
		
		float sum = dot(direction, octant);
		float3 octahedron = direction / sum;

		return 0.5f * float2(
			1.0f + octahedron.x + octahedron.z,
			1.0f + octahedron.z - octahedron.x
		);
	}

	float3 DecodePyramid(float2 encoded) {

		float2 oct = float2(
			(encoded.x - encoded.y),
			(encoded.x + encoded.y) - 1.0f
		);

		float2 absolute = abs(oct);

		float3 direction = float3(oct.x, 1.0f - absolute.x - absolute.y, oct.y);
		return normalize(direction);
	}

}

#endif
