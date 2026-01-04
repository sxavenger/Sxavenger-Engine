#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderPass.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <list>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassContext class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassContext {
public:

	//* render option *//

	void Render(const DirectXQueueContext* context, const FBaseRenderPass::Config& config);

	//* pass option *//

	void Emplace(std::unique_ptr<FBaseRenderPass>&& pass);

	template <FRenderPass T>
	void Add() { Emplace(std::make_unique<T>()); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<std::unique_ptr<FBaseRenderPass>> passes_;

	//=========================================================================================
	// public methods
	//=========================================================================================

	static FBaseRenderPass::Config ApplyConfig(const FBaseRenderPass::Config& config);

};

SXAVENGER_ENGINE_NAMESPACE_END
