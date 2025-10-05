#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderPass.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassDeferredBase class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassForwardTransparent
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderPassForwardTransparent()           = default;
	~FRenderPassForwardTransparent() override = default;

	void Render(const DirectXQueueContext* context, const Config& config) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void BeginPassRenderTarget(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPassRenderTarget(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void ClearPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassStaticMesh(const DirectXQueueContext* context, const Config& config);

	void PassSkinnedMesh(const DirectXQueueContext* context, const Config& config);

};
