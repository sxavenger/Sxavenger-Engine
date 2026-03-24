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
// FRenderPassCanvas class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief UI描画Pass
class FRenderPassCanvas
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* render canvas pass *//

	void BeginRenderCanvasPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndRenderCanvasPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void RenderSprite(const DirectXQueueContext* context, const FRenderConfig& config);

	void RenderText(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
