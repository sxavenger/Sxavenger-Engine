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
// FRenderPassLightingComposite class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ライティング合成Pass
class FRenderPassLightingComposite
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

	//* transition lighting composite pass *//

	void TransitionLightingCompositePass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

};

SXAVENGER_ENGINE_NAMESPACE_END

