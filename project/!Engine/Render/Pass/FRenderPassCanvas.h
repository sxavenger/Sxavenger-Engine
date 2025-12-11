#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderPass.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassCanvas class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassCanvas
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

	void PassSprite(const DirectXQueueContext* context, const Config& config);

	void PassText(const DirectXQueueContext* context, const Config& config);

};
