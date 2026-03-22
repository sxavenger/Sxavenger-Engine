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
// FRenderPassTonemap class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Tonemap処理Pass
class FRenderPassTonemap
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

	//* tonemap pass *//

	void BeginTonemapPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndTonemapPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassTonemap(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
