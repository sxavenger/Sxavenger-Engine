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
class FRenderPassDeferredBase
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

	void PassStaticMesh(const DirectXQueueContext* context, const Config& config);

	void PassSkinnedMesh(const DirectXQueueContext* context, const Config& config);

	void BeginPassVelocity(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPassVelocity(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassVelocity(const DirectXQueueContext* context, const Config& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
