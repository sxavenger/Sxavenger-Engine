#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseBuffer.h"
#include "../Common/FRenderTexture.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FLightAccumulationBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 直接光/間接光のライティング結果を別々に蓄積するバッファ
class FLightAccumulationBuffer final
	: public FBaseBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ライティング蓄積の種別 (直接光/間接光)
	enum class Layout : uint8_t {
		Direct,
		Indirect
	};
	static inline const size_t kLayoutCount = magic_enum::enum_count<Layout>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 指定解像度で全レイアウト分のテクスチャを生成する
	//! @param[in] resolution バッファの解像度
	void Create(const Vector2ui& resolution) override;

	//* getter *//

	//! @brief 指定レイアウトの蓄積テクスチャを取得する
	//! @param[in] layout 取得するレイアウト
	//! @return 対応するレンダーテクスチャへの参照
	FRenderTexture& GetBuffer(Layout layout) { return buffers_[static_cast<size_t>(layout)]; }

	//! @brief 指定レイアウトのDXGIフォーマットを取得する
	//! @param[in] layout 取得するレイアウト
	//! @return 対応するDXGIフォーマット
	static DXGI_FORMAT GetFormat(Layout layout) { return kFormats[static_cast<size_t>(layout)]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const std::array<DXGI_FORMAT, kLayoutCount> kFormats;

	std::array<FRenderTexture, kLayoutCount> buffers_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
