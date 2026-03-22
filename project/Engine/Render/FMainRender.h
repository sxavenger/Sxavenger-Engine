#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "Scene/FScene.h"
#include "Buffer/FRenderTargetBuffer.h"
#include "Pass/FRenderConfig.h"
#include "Pass/FRenderPassContext.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FMainRender class
////////////////////////////////////////////////////////////////////////////////////////////
class FMainRender {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	//* render option *//

	void Render(const DirectXQueueContext* context, const DirectXWindowContext* window);

	void Present(const DirectXQueueContext* context, const Vector2ui& resolution = Configuration::GetConfig().resolution);
	void Present(const DirectXQueueContext* context, const DirectXWindowContext* window);

	//* getter *//

	FRenderTargetBuffer* GetBuffer() const { return buffer_.get(); }

	FScene* GetScene() const { return scene_.get(); }

	FRenderPassContext& GetContext() { return context_; }

	FRenderConfig& GetConfig() { return config_; }

	//* singleton *//

	static FMainRender* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* render pass context *//

	FRenderPassContext context_ = {};

	//* parameter *//

	std::unique_ptr<FRenderTargetBuffer> buffer_;
	std::unique_ptr<FScene> scene_;

	FRenderConfig config_ = {};



};

SXAVENGER_ENGINE_NAMESPACE_END
