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
// FMainBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 最終出力用のシーン/Canvasカラーテクスチャを管理するメインバッファ
class FMainBuffer final
	: public FBaseBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief メインバッファの用途別レイアウト
	enum class Layout : uint8_t {
		Scene,
		Canvas,
	};
	static inline const size_t kLayoutCount = magic_enum::enum_count<Layout>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 指定解像度で全レイアウト分のカラーテクスチャを生成する
	//! @param[in] resolution バッファの解像度
	void Create(const Vector2ui& resolution) override;

	//* getter *//

	//! @brief 指定レイアウトのカラーテクスチャを取得する
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
