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

	void Render(const DirectXQueueContext* context, const FRenderConfig& config) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* XeGTAO Pass *//

	void BeginAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndAntiAliasingPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassAntiAliasing_XeGTAO(const DirectXQueueContext* context, const FRenderConfig& config);

};


SXAVENGER_ENGINE_NAMESPACE_END
