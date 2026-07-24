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
// FRenderPassAntiAliasing
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief アンチエイリアシング処理Pass
class FRenderPassAntiAliasing
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* anti-aliasing pass *//

	void BeginAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassAntiAliasingFXAA(const DirectXQueueContext* context, const FRenderConfig& config);

	void PassAntiAliasingSMAA(const DirectXQueueContext* context, const FRenderConfig& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
