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

	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* environment pass *//

	void BeginEnvironmentPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndEnvironmentPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void RenderEnvironmentSkyLight(const DirectXQueueContext* context, const FRenderConfig& config);

	void RenderEnvironmentSkyAtmosphere(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
