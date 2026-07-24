#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FRenderConfig.h"
#include "FBaseRenderPass.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassContext class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderPassContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* render option *//

	void Render(const DirectXQueueContext* context, const FRenderConfig& config);

	//* render pass option *//

	void Insert(std::unique_ptr<FBaseRenderPass>&& pass);

	template <FRenderPass T>
	void Insert() { Insert(std::make_unique<T>()); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<std::unique_ptr<FBaseRenderPass>> passes_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static FRenderConfig ResolveConfig(const FRenderConfig& _config);

};

SXAVENGER_ENGINE_NAMESPACE_END
