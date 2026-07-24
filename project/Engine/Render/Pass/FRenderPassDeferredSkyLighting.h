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
// FRenderPassDeferredSkyLighting class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 環境光の描画Pass
class FRenderPassDeferredSkyLighting
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief 天球/大気による環境光と天空可視性を計算し, 蓄積する (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* environment pass *//

	//! @brief 環境光パスの開始処理(バリア遷移等)を行う
	void BeginEnvironmentPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief 環境光パスの終了処理を行う
	void EndEnvironmentPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief 天球ライト(SkyLight)による環境光を描画する
	void RenderEnvironmentSkyLight(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 大気散乱(SkyAtmosphere)による環境光を描画する
	void RenderEnvironmentSkyAtmosphere(const DirectXQueueContext* context, const FRenderConfig& config);

	//* sky visibility pass *//

	//! @brief 天空可視性パスの開始処理を行う
	void BeginSkyVisibilityPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief 天空可視性パスの終了処理を行う
	void EndSkyVisibilityPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief 天空可視性のReservoirを初期生成する
	void InitalReservoirSkyVisibility(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 天空可視性のReservoirを空間方向に再利用する
	void SpatialReservoirSkyVisibility(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 天空可視性の履歴を統合する(テンポラル)
	void HistorySkyVisibility(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 天空可視性から最終結果を解決する
	void SolveSkyVisibility(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
