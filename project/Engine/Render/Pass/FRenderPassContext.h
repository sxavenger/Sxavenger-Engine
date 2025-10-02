#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderPass.h"

//* c++
#include <list>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassContext class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

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
	// private methods
	//=========================================================================================

	static FBaseRenderPass::Config ApplyConfig(const FBaseRenderPass::Config& config);

};
