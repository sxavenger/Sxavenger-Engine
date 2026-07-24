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
// FRenderPassDeferredBase class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief GBufferへの描画Pass
class FRenderPassDeferredBase
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief 不透明メッシュ/デカール/モーションベクトルをGBufferへ書き込む (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* opaque mesh render pass *//

	//! @brief 不透明メッシュパスの開始処理(バリア遷移/レンダーターゲット設定)を行う
	void BeginOpaqueMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief 不透明メッシュパスの終了処理を行う
	void EndOpaqueMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief 静的(スキンなし)メッシュをGBufferへ描画する
	void RenderStaticMesh(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief スキニングメッシュをGBufferへ描画する
	void RenderSkinnedMesh(const DirectXQueueContext* context, const FRenderConfig& config);

	//* decal render pass *//

	//! @brief デカールパスの開始処理を行う
	void BeginDecalRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief デカールパスの終了処理を行う
	void EndDecalRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief デカールをGBufferへ投影描画する
	void RenderDecal(const DirectXQueueContext* context, const FRenderConfig& config);

	//* motion vector pass *//

	//! @brief モーションベクトルパスの開始処理を行う
	void BeginMotionVectorPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief モーションベクトルパスの終了処理を行う
	void EndMotionVectorPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief 前フレームとの差分からモーションベクトルを算出・書き込む
	void PassMotionVector(const DirectXQueueContext* context, const FRenderConfig& config);

	//* lighting only *//

	//! @brief ライティングのみ表示モード用にGBufferのAlbedoを白で埋める
	void LightingOnly(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

};

SXAVENGER_ENGINE_NAMESPACE_END


