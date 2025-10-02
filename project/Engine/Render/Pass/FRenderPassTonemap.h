#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderPass.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassTonemap class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassTonemap
	: public FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderPassTonemap()           = default;
	~FRenderPassTonemap() override = default;

	void Render(const DirectXQueueContext* context, const Config& config) override;


};
