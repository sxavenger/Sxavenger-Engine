#pragma once
// Weighted Reservoir Sampling for ReSTIR

/* # reference
 - [NVIDIA ReSTIR Direct Illumination](https://research.nvidia.com/sites/default/files/pubs/2020-07_Spatiotemporal-reservoir-resampling/ReSTIR.pdf)
 - [NVIDIA ReSTIR Global Illumination](https://d1qx31qr3h6wln.cloudfront.net/publications/ReSTIR%20GI.pdf)
 - [NVIDIA ReSTIR Path Tracing](https://d1qx31qr3h6wln.cloudfront.net/publications/sig22_GRIS.pdf)
*/

//!< sky visibility 用に改良.

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../../Library/ACES.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// ReservoirLib namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace ReservoirLib {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Sample structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Sample {

		//=========================================================================================
		// public variables
		//=========================================================================================

		float3 direction; //!< sample direction
		float3 radiance;  //!< sample radiance
		float pdf;        //!< sample pdf
		
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Reservoir structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Reservoir {
	
		//=========================================================================================
		// public variables
		//=========================================================================================
	
		Sample sample; //!< sample data
		float weight; //!< reservoir weight
		float w; //!< sum of weights
		uint m; //!< number of samples
	
		//=========================================================================================
		// public methods
		//=========================================================================================
	
		//! @brief sampleの更新
		//! @param s[in] new sample
		//! @param w[in] new sample weight
		//! @param r[in] random value [0, 1]
		void Update(Sample s, float w, float r) {
			weight += w;
			m++;
			
			if (r < w / weight) {
				sample = s;
			}
		}
	
		//! @brief reservoirの統合
		//! @param reservoir[in] merge source reservoir
		//! @param p_hat[in] estimated sample weight of the target reservoir
		//! @param r[in] random value [0, 1]
		void Merge(Reservoir reservoir, float p_hat, float r) {
			uint m0 = m;
			Update(reservoir.sample, p_hat * reservoir.w * reservoir.m, r);
			m = m0 + reservoir.m;
		}

		void Normalize() {
			float p_hat = dot(sample.radiance, ACES::AP1_RGB2Y);
			w = (p_hat > 0.0f && m > 0) ? weight / (m * p_hat) : 0.0f;
		}

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// methods
	////////////////////////////////////////////////////////////////////////////////////////////

	uint FlattenIndex(uint2 index, uint2 dimension) {
		return index.x + index.y * dimension.x;
	}
	
}
