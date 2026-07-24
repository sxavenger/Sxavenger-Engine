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
//! @brief 天球(Sky)ライティングのReSTIR用Reservoirと履歴テクスチャを管理するバッファ
class FSkyReservoirBuffer final
	: public FBaseBuffer {
public:

	PUSH_GPU_BUFFER_ALIGNAS

	////////////////////////////////////////////////////////////////////////////////////////////
	// Reservoir structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Sky ReSTIRのReservoir(サンプルと重み情報)を保持するGPU構造体
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

	using Reservoir = BaseReservoir<7>; //!< ReservoirLib::Sample構造体のsizeに合わせる.

	////////////////////////////////////////////////////////////////////////////////////////////
	// LayoutReservoir enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Reservoirの再利用段階 (初期/空間再利用)
	enum class LayoutReservoir : uint8_t {
		Initial,
		Spatial0,
		Spatial1,
	};
	static inline const size_t kLayoutReservoirCount = magic_enum::enum_count<LayoutReservoir>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// LayoutTexture enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 保持するテクスチャの種別
	enum class LayoutTexture : uint8_t {
		History,
	};
	static inline const size_t kLayoutTextureCount = magic_enum::enum_count<LayoutTexture>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 指定解像度で全段階分のReservoir/テクスチャを生成する
	//! @param[in] resolution バッファの解像度
	void Create(const Vector2ui& resolution) override;

	//* getter *//

	//! @brief 指定段階のReservoirバッファを取得する
	//! @param[in] layout 取得する段階
	//! @return 対応するReservoirバッファへの参照
	DxObject::UnorderedDimensionBuffer<Reservoir>& GetReservoir(LayoutReservoir layout) { return reservoirs_[static_cast<size_t>(layout)]; }

	//! @brief 指定レイアウトのテクスチャを取得する
	//! @param[in] layout 取得するレイアウト
	//! @return 対応するレンダーテクスチャへの参照
	FRenderTexture& GetBuffer(LayoutTexture layout) { return buffers_[static_cast<size_t>(layout)]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<DxObject::UnorderedDimensionBuffer<Reservoir>, kLayoutReservoirCount> reservoirs_ = {};
	std::array<FRenderTexture, kLayoutTextureCount> buffers_ = {};
	
};

SXAVENGER_ENGINE_NAMESPACE_END
