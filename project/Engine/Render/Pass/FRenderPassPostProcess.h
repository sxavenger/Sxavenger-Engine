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
// FRenderPassPostProcess class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ポストプロセス処理Pass
class FRenderPassPostProcess
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief 各種ポストエフェクト(Bloom/露出調整/色収差等)を適用する (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* post process pass *//

	//! @brief ポストプロセスパスの開始処理を行う
	void BeginPostProcessPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief ポストプロセスパスの終了処理を行う
	void EndPostProcessPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief シーン全体に適用するグローバルなポストエフェクトを処理する
	void PostProcessGlobal(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief ボリューム範囲内で適用するポストエフェクトを処理する
	void PostProcessVolume(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 局所的に適用するポストエフェクトを処理する
	void PostProcessLocal(const DirectXQueueContext* context, const FRenderConfig& config);

};



SXAVENGER_ENGINE_NAMESPACE_END
