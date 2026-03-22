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
// FRenderPassReflection class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassReflection
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

	void BeginPassReflection(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void EndPassReflection(const DirectXQueueContext* context, FRenderTargetBuffer* buffer);

	void PassReflection(const DirectXQueueContext* context, const Config& config);

	void ProcessReflectionCalculate(const DirectXQueueContext* context, const Config& config);

	void TransitionReflectionPass(const DirectXQueueContext* context, const Config& config);

};

SXAVENGER_ENGINE_NAMESPACE_END

