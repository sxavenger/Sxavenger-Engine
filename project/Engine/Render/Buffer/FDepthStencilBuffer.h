#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseBuffer.h"
#include "../Common/FDepthStencilTexture.h"

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
// FDepthStencilBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief シーン用/Canvas用の深度ステンシルテクスチャをまとめて管理するバッファ
class FDepthStencilBuffer final
	: public FBaseBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 深度ステンシルの用途別レイアウト
	enum class Layout : uint8_t {
		Scene,
		Canvas,
	};
	static inline const size_t kLayoutCount = magic_enum::enum_count<Layout>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 指定解像度で全レイアウト分の深度ステンシルを生成する
	//! @param[in] resolution バッファの解像度
	void Create(const Vector2ui& resolution) override;

	//* getter *//

	//! @brief 指定レイアウトの深度ステンシルテクスチャを取得する
	//! @param[in] layout 取得するレイアウト
	//! @return 対応する深度ステンシルテクスチャへの参照
	FDepthStencilTexture& GetBuffer(Layout layout) { return buffers_[static_cast<size_t>(layout)]; }

	//! @brief 指定レイアウトのDXGIフォーマットを取得する
	//! @param[in] layout 取得するレイアウト
	//! @return 対応するDXGIフォーマット
	static DXGI_FORMAT GetFormat(Layout layout) { return kFormats[static_cast<size_t>(layout)]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const std::array<DXGI_FORMAT, kLayoutCount> kFormats;

	std::array<FDepthStencilTexture, kLayoutCount> buffers_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
