#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseBuffer.h"

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
// FReservoirBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ReSTIRの各段階(Initial/Temporal/Spatial)のReservoirをGPUバッファとして保持するバッファ
class FReservoirBuffer final
	: public FBaseBuffer {
public:

	PUSH_GPU_BUFFER_ALIGNAS

	////////////////////////////////////////////////////////////////////////////////////////////
	// Reservoir structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ReSTIRのReservoir(サンプルと重み情報)を保持するGPU構造体
	//! @tparam N サンプルデータの要素数
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

	using Reservoir = BaseReservoir<12>; //!< ReSTIR::Sample構造体のsizeに合わせる.

	////////////////////////////////////////////////////////////////////////////////////////////
	// Moment structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Reservoirのモーメント(オフセット/index)情報を保持する構造体
	struct Moment {
	public:

		uint32_t offset; //!< moment offset
		uint32_t index;  //!< moment index

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ReSTIRのReservoir再利用段階
	enum class Layout : uint8_t {
		Initial,
		Temporal,
		Spatial,
	};
	static inline const size_t kLayoutCount = magic_enum::enum_count<Layout>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 指定解像度で全段階分のReservoirバッファを生成する
	//! @param[in] resolution バッファの解像度
	void Create(const Vector2ui& resolution) override;

	//* getter *//

	//! @brief 指定段階のReservoirバッファを取得する
	//! @param[in] layout 取得する段階
	//! @return 対応するReservoirバッファへの参照
	DxObject::UnorderedDimensionBuffer<Reservoir>& GetReservoir(Layout layout) { return reservoirs_[static_cast<size_t>(layout)]; }

	//! @brief モーメントバッファを取得する
	//! @return モーメントバッファへの参照
	DxObject::UnorderedDimensionBuffer<Moment>& GetMoment() { return moment_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<DxObject::UnorderedDimensionBuffer<Reservoir>, kLayoutCount> reservoirs_ = {};
	DxObject::UnorderedDimensionBuffer<Moment> moment_;

};

SXAVENGER_ENGINE_NAMESPACE_END
