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
// FRenderPassDeferredBase class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassForwardTransparent
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

	void BeginPassRenderTarget(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPassRenderTarget(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassStaticMeshOpaque(const DirectXQueueContext* context, const Config& config);
	void PassSkinnedMeshOpaque(const DirectXQueueContext* context, const Config& config);

	void PassStaticMeshTransparent(const DirectXQueueContext* context, const Config& config);
	void PassSkinnedMeshTransparent(const DirectXQueueContext* context, const Config& config);

	void TransitionTransparentPass(const DirectXQueueContext* context, const Config& config);

	void PassParticles(const DirectXQueueContext* context, const Config& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
