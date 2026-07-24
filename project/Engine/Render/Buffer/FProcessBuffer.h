#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Common/FProcessTexture.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ポストプロセスのピンポン処理用に複数のプロセステクスチャを循環管理するバッファ
class FProcessBuffer final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FProcessBuffer() = default;

	//* container option *//

	//! @brief 指定数のプロセステクスチャを生成する
	//! @param[in] size     生成するテクスチャ数
	//! @param[in] argument テクスチャ生成に渡す引数
	void Create(uint8_t size, const FProcessTexture::Argument& argument);

	//! @brief テクスチャ配列の容量を予約する
	//! @param[in] size 予約するサイズ
	void Reserve(uint8_t size) { textures_.reserve(size); }

	//* process option *//

	//! @brief 外部テクスチャを現在のプロセステクスチャへ取り込む
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] texture 取り込み元テクスチャ
	void Import(const DirectXQueueContext* context, FRenderTexture* texture);

	//! @brief 現在のプロセステクスチャを外部テクスチャへ書き出す
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] texture 書き出し先テクスチャ
	void Export(const DirectXQueueContext* context, FRenderTexture* texture);

	//! @brief 現在のテクスチャindexを進める (ピンポン切り替え)
	//! @param[in] count 進める数
	void Next(uint8_t count = 1);

	//! @brief 現在の(書き込み対象)プロセステクスチャを取得する
	//! @return 現在のプロセステクスチャへの参照
	FProcessTexture& GetCurrentTexture();

	//! @brief 指定数だけ前の(読み込み対象)プロセステクスチャを取得する
	//! @param[in] previous 遡る数
	//! @return 過去のプロセステクスチャへの参照
	FProcessTexture& GetPreviousTexture(uint8_t previous = 1);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* textures *//

	std::vector<FProcessTexture> textures_;
	uint8_t currentIndex_ = 0;

};

SXAVENGER_ENGINE_NAMESPACE_END
