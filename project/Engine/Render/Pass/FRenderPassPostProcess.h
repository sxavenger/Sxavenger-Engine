#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderPass.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassPostProcess class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassPostProcess
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderPassPostProcess()           = default;
	~FRenderPassPostProcess() override = default;

	void Render(const DirectXQueueContext* context, const Config& config) override;


};
