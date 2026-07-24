#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FRenderConfig.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseRenderPass class
////////////////////////////////////////////////////////////////////////////////////////////
class FBaseRenderPass {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FBaseRenderPass()          = default;
	virtual ~FBaseRenderPass() = default;

	virtual void Init() {};

	virtual void Render(const DirectXQueueContext* context, const FRenderConfig& config) = 0;

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

	static void BeginRenderPass(const DirectXQueueContext* context, const std::string& pass, const FRenderConfig& config);

	static void EndRenderPass(const DirectXQueueContext* context);

	static void BeginEvent(const DirectXQueueContext* context, const std::string& event);

	static void EndEvent(const DirectXQueueContext* context);

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPass concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept FRenderPass = std::derived_from<T, FBaseRenderPass> && !std::is_same_v<T, FBaseRenderPass>;

SXAVENGER_ENGINE_NAMESPACE_END
