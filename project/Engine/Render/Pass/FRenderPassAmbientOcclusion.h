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
// FRenderPassAmbientOcclusion class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Ambient-Occlusionの描画Pass
class FRenderPassAmbientOcclusion
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief 深度/法線からアンビエントオクルージョンを計算する (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* XeGTAO Pass *//

	//! @brief AOパスの開始処理(バリア遷移等)を行う
	void BeginAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief AOパスの終了処理を行う
	void EndAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief XeGTAOによるアンビエントオクルージョンを計算する
	void PassAntiAliasing_XeGTAO(const DirectXQueueContext* context, const FRenderConfig& config);

};


SXAVENGER_ENGINE_NAMESPACE_END
