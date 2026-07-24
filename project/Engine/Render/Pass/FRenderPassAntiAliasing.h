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
// FRenderPassAntiAliasing
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief アンチエイリアシング処理Pass
class FRenderPassAntiAliasing
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief 設定されたアンチエイリアス手法(FXAA/SMAA)を適用する (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* anti-aliasing pass *//

	//! @brief AAパスの開始処理(バリア遷移等)を行う
	void BeginAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief AAパスの終了処理を行う
	void EndAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief FXAAによるアンチエイリアスを適用する
	void PassAntiAliasingFXAA(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief SMAAによるアンチエイリアスを適用する
	void PassAntiAliasingSMAA(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
