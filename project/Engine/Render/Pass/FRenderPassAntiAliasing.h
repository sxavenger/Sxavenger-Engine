#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderPass.h"

//* engine
#include <Engine/Foundation.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassAntiAliasing class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassAntiAliasing
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Render(const DirectXQueueContext* context, const Config& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginProcessAntiAliasing(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndProcessAntiAliasing(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void ProcessFXAA(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void ProcessSMAA(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

};

SXAVENGER_ENGINE_NAMESPACE_END
