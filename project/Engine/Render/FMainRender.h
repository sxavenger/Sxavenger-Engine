#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "Scene/FScene.h"
#include "Buffer/FRenderTargetBuffer.h"
#include "Pass/FRenderConfig.h"
#include "Pass/FRenderPassContext.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FMainRender class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief エンジンのメイン描画を統括するクラス. シーン/バッファ/パス構成を保持する (シングルトン)
class FMainRender {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 描画バッファ/シーン/パス構成を初期化する
	void Init();

	//! @brief 保持リソースを破棄する
	void Term();

	//* render option *//

	//! @brief 登録された全パスを実行し, 1フレームを描画する
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] window  出力先ウィンドウコンテキスト
	void Render(const DirectXQueueContext* context, const DirectXWindowContext* window);

	//! @brief 描画結果を指定解像度でスワップチェーンへ表示する
	//! @param[in] context    DirectXのキューコンテキスト
	//! @param[in] resolution 出力解像度
	void Present(const DirectXQueueContext* context, const Vector2ui& resolution = Configuration::GetConfig().resolution);
	//! @brief 描画結果を指定ウィンドウへ表示する
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] window  出力先ウィンドウコンテキスト
	void Present(const DirectXQueueContext* context, const DirectXWindowContext* window);

	//* getter *//

	//! @brief 描画対象のレンダーターゲットバッファを取得する
	//! @return バッファへのポインタ
	FRenderTargetBuffer* GetBuffer() const { return buffer_.get(); }

	//! @brief 描画対象のシーンを取得する
	//! @return シーンへのポインタ
	FScene* GetScene() const { return scene_.get(); }

	//! @brief 描画パスコンテキストを取得する (パスの追加等に使用)
	//! @return パスコンテキストへの参照
	FRenderPassContext& GetContext() { return context_; }

	//! @brief 描画設定を取得する
	//! @return 描画設定への参照
	FRenderConfig& GetConfig() { return config_; }

	//* singleton *//

	//! @brief シングルトンインスタンスを取得する
	//! @return インスタンスへのポインタ
	static FMainRender* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* render pass context *//

	FRenderPassContext context_ = {};

	//* parameter *//

	std::unique_ptr<FRenderTargetBuffer> buffer_;
	std::unique_ptr<FScene> scene_;

	FRenderConfig config_ = {};



};

SXAVENGER_ENGINE_NAMESPACE_END
