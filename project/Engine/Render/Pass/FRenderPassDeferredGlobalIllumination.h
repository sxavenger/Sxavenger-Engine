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
// FRenderPassDeferredGlobalIllumination class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 間接光の描画Pass
class FRenderPassDeferredGlobalIllumination // [FRenderPassDeferredGlobalIllumination]に変更するかも...?
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief GIに必要なレイトレ資源等を初期化する (FBaseRenderPassのoverride)
	void Init() override;

	//! @brief 設定されたGI手法(Lux/ReSTIR)で間接光を計算し, 蓄積する (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* global illumination pass *//

	//! @brief GIパスの開始処理(バリア遷移等)を行う
	void BeginGlobalIlluminationPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief GIパスの終了処理を行う
	void EndGlobalIlluminationPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//* Lux pass *//

	//! @brief Lux GIパスの開始処理を行う
	void BeginPassLux(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief Lux GIパスの終了処理を行う
	void EndPassLux(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief プローブからレイを飛ばして放射輝度を収集する
	void PassLuxProbeTrace(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 過去フレームの結果と履歴を統合する(テンポラル)
	void PassLuxHistory(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 収集した放射輝度から最終的な間接光を解決する
	void PassLuxSolve(const DirectXQueueContext* context, const FRenderConfig& config);

	//* ReSTIR pass *//

	//! @brief ReSTIR GIパスの開始処理を行う
	void BeginPassReSTIR(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief ReSTIR GIパスの終了処理を行う
	void EndPassReSTIR(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief Reservoirを初期状態にリセットする
	void PassReSTIRResetReservoir(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 初期サンプルからReservoirを生成する
	void PassReSTIRInitialReservoir(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 過去フレームのReservoirを時間方向に再利用する
	void PassReSTIRTemporalReuse(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief Reservoirから最終的な間接光を解決する
	void PassReSTIRSolve(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
