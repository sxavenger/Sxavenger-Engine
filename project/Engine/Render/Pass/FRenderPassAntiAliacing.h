#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderPass.h"

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
