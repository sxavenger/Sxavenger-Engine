#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FRenderConfig.h"
#include "FBaseRenderPass.h"
#include "../Buffer/FRenderTargetBuffer.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassCanvas class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief UI描画Pass
class FRenderPassCanvas
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief スプライト/テキストなどのUIをCanvasへ描画する (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* render canvas pass *//

	//! @brief Canvasパスの開始処理(バリア遷移/レンダーターゲット設定)を行う
	void BeginRenderCanvasPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief Canvasパスの終了処理を行う
	void EndRenderCanvasPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief スプライトを描画する
	void RenderSprite(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief テキストを描画する
	void RenderText(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
