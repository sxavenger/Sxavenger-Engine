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
// FRenderPassLightingComposite class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ライティング合成Pass
class FRenderPassLightingComposite
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief 直接光/間接光/環境光の蓄積結果を合成し, シーンカラーを生成する (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* transition lighting composite pass *//

	//! @brief 蓄積バッファを合成し, メインバッファへ書き込む
	void TransitionLightingCompositePass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

};

SXAVENGER_ENGINE_NAMESPACE_END

