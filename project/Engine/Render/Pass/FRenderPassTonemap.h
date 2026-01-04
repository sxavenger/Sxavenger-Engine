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

SXAVENGER_ENGINE_NAMESPACE_END
