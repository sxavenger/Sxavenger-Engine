#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseBuffer.h"
#include "../Common/FRenderTexture.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DirectXAlignment.h>
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FSkyReservoirBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FSkyReservoirBuffer final
	: public FBaseBuffer {
public:

	PUSH_GPU_BUFFER_ALIGNAS

	////////////////////////////////////////////////////////////////////////////////////////////
	// Reservoir structure
	////////////////////////////////////////////////////////////////////////////////////////////
	template <size_t N>
	struct GPU_BUFFER_ALIGNAS BaseReservoir {
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

	using Reservoir = BaseReservoir<7>; //!< ReservoirLib::Sample構造体のsizeに合わせる.

	////////////////////////////////////////////////////////////////////////////////////////////
	// LayoutReservoir enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class LayoutReservoir : uint8_t {
		Initial,
		Spatial0,
		Spatial1,
	};
	static inline const size_t kLayoutReservoirCount = magic_enum::enum_count<LayoutReservoir>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// LayoutTexture enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class LayoutTexture : uint8_t {
		History,
	};
	static inline const size_t kLayoutTextureCount = magic_enum::enum_count<LayoutTexture>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const Vector2ui& resolution) override;

	//* getter *//

	DxObject::UnorderedDimensionBuffer<Reservoir>& GetReservoir(LayoutReservoir layout) { return reservoirs_[static_cast<size_t>(layout)]; }

	FRenderTexture& GetBuffer(LayoutTexture layout) { return buffers_[static_cast<size_t>(layout)]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<DxObject::UnorderedDimensionBuffer<Reservoir>, kLayoutReservoirCount> reservoirs_ = {};
	std::array<FRenderTexture, kLayoutTextureCount> buffers_ = {};
	
};

SXAVENGER_ENGINE_NAMESPACE_END
