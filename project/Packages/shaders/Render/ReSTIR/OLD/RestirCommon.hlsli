#pragma once
// Weighted Reservoir Sampling for ReSTIR

/* reference
 - [NVIDIA ReSTIR Direct Illumination](https://research.nvidia.com/sites/default/files/pubs/2020-07_Spatiotemporal-reservoir-resampling/ReSTIR.pdf)
 - [NVIDIA ReSTIR Global Illumination](https://d1qx31qr3h6wln.cloudfront.net/publications/ReSTIR%20GI.pdf)
 - [NVIDIA ReSTIR Path Tracing](https://d1qx31qr3h6wln.cloudfront.net/publications/sig22_GRIS.pdf)
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "../../Library/RandomLib.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Sample structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Sample {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float3 xv; //!< visible point position
	float3 nv; //!< visible point normal
	float3 xs; //!< sample point position
	float3 ns; //!< sample point normal
	float3 lo; //!< sample radiance
	float pdf;
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// Reservoir structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Reservoir {

	//=========================================================================================
	// public variables
	//=========================================================================================

	Sample sample;
	float weight; //!< wsum
	float w;      //!< W
	uint m;       //!< M

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief sample update
	//! @param s[in] new sample
	//! @param w[in] new sample weight
	//! @param r[in] random value [0, 1]
	void Update(Sample s, float w, float r) {
		weight += w;
		m++;
		
		if (r < w * rcp(weight)) {
			sample = s;
		}

	}

	//! @brief reservoir merge
	//! @param reservoir[in] merge source reservoir
	//! @param p_hat[in] estimated sample weight of the target reservoir
	//! @param r[in] random value [0, 1]
	void Merge(Reservoir reservoir, float p_hat, float r) {
		uint m0 = m;
		Update(reservoir.sample, p_hat * reservoir.w * reservoir.m, r);
		m = m0 + reservoir.m;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// Moment structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Moment {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint offset;
	uint index;

	//=========================================================================================
	// public methods
	//=========================================================================================

	static Moment GetDefault(uint2 pixel) {
		Moment moment = (Moment)0;
		moment.offset = Xorshift::xorshift32(pixel.x * pixel.y) + 1;

		return moment;
	}

	uint GetRandamizeSampleIndex(uint i, uint maxSampleCount) {
		const uint kDivision = maxSampleCount / 16;
		uint divisionIndex = kDivision * ((index + i + offset) % maxSampleCount);
	
		return (divisionIndex % maxSampleCount) + (divisionIndex / maxSampleCount);
	}

	bool CheckCompleteSample(uint maxSampleCount) {
		return index >= maxSampleCount;
	}
	
};
