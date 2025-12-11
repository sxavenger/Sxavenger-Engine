#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FPresenter.h"
#include "FRenderTargetBuffer.h"
#include "Scene/FScene.h"
#include "Pass/FRenderPassContext.h"

//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FMainRender class
////////////////////////////////////////////////////////////////////////////////////////////
class FMainRender {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FMainRender()  = default;
	~FMainRender() = default;

	void Init();

	void Term();

	//* option *//

	void Render(const DirectXQueueContext* context, DirectXWindowContext* window);

	void PresentMain(const DirectXQueueContext* context);

	//* getter *//

	FRenderTargetBuffer* GetTextures() const { return buffer_.get(); }

	FScene* GetScene() const { return scene_.get(); }

	FRenderPassContext* GetContext() const { return context_.get(); }

	FBaseRenderPass::Config& GetConfig() { return config_; }

	//* singleton *//

	static FMainRender* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FRenderTargetBuffer> buffer_;
	std::unique_ptr<FScene>              scene_;

	std::unique_ptr<FRenderPassContext> context_;

	FBaseRenderPass::Config config_ = {};

};
