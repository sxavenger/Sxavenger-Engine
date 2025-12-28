#pragma once
// Weighted Reservoir Sampling for ReSTIR + Dynamic Diffuse Global Illumination

/* reference
* ReSTIR
 - [NVIDIA ReSTIR Direct Illumination](https://research.nvidia.com/sites/default/files/pubs/2020-07_Spatiotemporal-reservoir-resampling/ReSTIR.pdf)
 - [NVIDIA ReSTIR Global Illumination](https://d1qx31qr3h6wln.cloudfront.net/publications/ReSTIR%20GI.pdf)
 - [NVIDIA ReSTIR Path Tracing](https://d1qx31qr3h6wln.cloudfront.net/publications/sig22_GRIS.pdf)

* Dynamic Diffuse Global Illumination
 - [NVIDIA Dynamic Diffuse Global Illumination with Ray-Traced Irradiance Fields](https://jcgt.org/published/0008/02/01/paper.pdf)
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

	float3 lo; //!< radiance
	float pdf; //!< pdf
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// Reservoir structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Reservoir {

	//=========================================================================================
	// public variables
	//=========================================================================================

	Sample sample;    //!< sample
	float weight_sum; //!< weight_sum
	float w;          //!< W
	uint m;           //!< M

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief sample update
	//! @param s[in] new sample
	//! @param w[in] new sample weight
	//! @param r[in] random value [0, 1]
	void Update(Sample s, float w, float r) {
		weight_sum += w;
		m++;
		
		if (r < w / weight_sum) {
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
