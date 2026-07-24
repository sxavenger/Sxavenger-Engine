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
// FRenderPassForwardTransparent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 透過物体の描画Pass
class FRenderPassForwardTransparent
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief 透過メッシュを順序独立透過(OIT)で描画する (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pass enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 透過描画の内部パス種別 (深度事前描画/透過本描画)
	enum class Pass : uint8_t {
		DepthPrepass,
		TransparentPass,
	};

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* depth pre-pass *//

	//! @brief 深度プリパスの開始処理を行う
	void BeginDepthPrepass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief 深度プリパスの終了処理を行う
	void EndDepthPrepass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//* transparent mesh render pass *//

	//! @brief 透過メッシュ描画パスの開始処理を行う
	void BeginTransparentMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief 透過メッシュ描画パスの終了処理を行う
	void EndTransparentMeshRenderPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//* transition transparent pass *//

	//! @brief OITの累積結果を合成し, メインバッファへ反映する
	void TransitionTransparentPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//* mesh render helper methods *//

	//! @brief 静的メッシュを指定パスで描画する
	//! @param[in] pass 描画対象の内部パス
	void RenderStaticMesh(const DirectXQueueContext* context, const FRenderConfig& config, Pass pass);

	//! @brief スキニングメッシュを指定パスで描画する
	//! @param[in] pass 描画対象の内部パス
	void RenderSkinnedMesh(const DirectXQueueContext* context, const FRenderConfig& config, Pass pass);

};

SXAVENGER_ENGINE_NAMESPACE_END
