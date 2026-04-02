#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseBuffer.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DirectXAlignment.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FReservoirBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FReservoirBuffer final
	: public FBaseBuffer {
public:

	PUSH_GPU_BUFFER_ALIGNAS

	////////////////////////////////////////////////////////////////////////////////////////////
	// Reservoir structure
	////////////////////////////////////////////////////////////////////////////////////////////
	template <size_t N>
	struct GPU_BUFFER_ALIGNAS Reservoir {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::array<BYTE, 4 * N> sample; //!< sample data
		float weight;                   //!< reservoir weight
		float w;                        //!< sum of weights
		uint32_t m;                     //!< number of samples

	};

	POP_GPU_BUFFER_ALIGNAS

	////////////////////////////////////////////////////////////////////////////////////////////
	// Moment structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Moment {
	public:

		uint32_t offset; //!< moment offset
		uint32_t index;  //!< moment index

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Layout : uint8_t {
		Initialize,
		Temporal,
		Spatial,
	};
	static inline const size_t kLayoutCount = magic_enum::enum_count<Layout>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const Vector2ui& resolution) override;

	//* getter *//



private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};

SXAVENGER_ENGINE_NAMESPACE_END
