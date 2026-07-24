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
// FRenderPassTonemap class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Tonemap処理Pass
class FRenderPassTonemap
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief HDRのシーンカラーをLDRへトーンマップする (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* tonemap pass *//

	//! @brief トーンマップパスの開始処理を行う
	void BeginTonemapPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief トーンマップパスの終了処理を行う
	void EndTonemapPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief トーンマップを適用してLDRへ変換する
	void PassTonemap(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
