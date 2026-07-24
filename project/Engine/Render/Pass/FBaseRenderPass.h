#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FRenderConfig.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseRenderPass class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 個々の描画パス(ライティング/ポストプロセス等)の基底クラス
class FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FBaseRenderPass()          = default;
	virtual ~FBaseRenderPass() = default;

	//! @brief パスの初期化を行う (必要な派生クラスのみ実装)
	virtual void Init() {};

	//! @brief このパスの描画処理を実行する (派生クラスで実装)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	virtual void Render(const DirectXQueueContext* context, const FRenderConfig& config) = 0;

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//! @brief パスの開始処理(デバッグイベント発行等)を行う
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] pass    パスの識別名
	//! @param[in] config  描画設定
	static void BeginRenderPass(const DirectXQueueContext* context, const std::string& pass, const FRenderConfig& config);

	//! @brief パスの終了処理を行う
	//! @param[in] context DirectXのキューコンテキスト
	static void EndRenderPass(const DirectXQueueContext* context);

	//! @brief デバッグ用のイベント区間を開始する
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] event   イベント名
	static void BeginEvent(const DirectXQueueContext* context, const std::string& event);

	//! @brief デバッグ用のイベント区間を終了する
	//! @param[in] context DirectXのキューコンテキスト
	static void EndEvent(const DirectXQueueContext* context);

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPass concept
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief FBaseRenderPassを継承した具象パス型のみを受け付けるconcept
template <class T>
concept FRenderPass = std::derived_from<T, FBaseRenderPass> && !std::is_same_v<T, FBaseRenderPass>;

SXAVENGER_ENGINE_NAMESPACE_END
