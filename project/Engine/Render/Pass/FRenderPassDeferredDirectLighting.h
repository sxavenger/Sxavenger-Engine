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
// FRenderPassDeferredDirectLighting class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 直接光の描画Pass
class FRenderPassDeferredDirectLighting
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	//! @brief GBufferを入力に各ライトの直接光を計算し, Light Accumulation Bufferへ加算する (FBaseRenderPassのoverride)
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] config  描画設定
	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* direct lighting pass *//

	//! @brief 直接光パスの開始処理(バリア遷移/レンダーターゲット設定)を行う
	void BeginDirectLightingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief 直接光パスの終了処理(バリア遷移)を行う
	void EndDirectLightingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	//! @brief ライトの影響を受けない要素(emissive等)を描画する
	void PassUnlit(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 全ての平行光源の寄与を描画・加算する
	void PassDirectionalLight(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 全ての点光源の寄与を描画・加算する
	void PassPointLight(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 全てのスポットライトの寄与を描画・加算する
	void PassSpotLight(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 全ての矩形光源の寄与を描画・加算する
	void PassRectLight(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 天球ライト(SkyLight)の寄与を描画・加算する
	void PassSkyLight(const DirectXQueueContext* context, const FRenderConfig& config);

	//! @brief 大気散乱(SkyAtmosphere)の寄与を描画・加算する
	void PassSkyAtmosphere(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
