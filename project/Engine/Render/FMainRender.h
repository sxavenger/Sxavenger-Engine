#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FPresenter.h>
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/Scene/FSceneRenderer.h>
#include <Engine/Render/Canvas/FCanvasRenderer.h>

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

	FSceneRenderer* GetRenderer() const { return sceneRenderer_.get(); }

	FScene* GetScene() const { return scene_.get(); }

	FCanvasRenderer* GetCanvas() const { return canvasRenderer_.get(); }

	FSceneRenderer::Config& GetConfig() { return config_; }

	//* singleton *//

	static FMainRender* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FRenderTargetBuffer> buffer_;
	std::unique_ptr<FSceneRenderer>      sceneRenderer_;
	std::unique_ptr<FScene>              scene_;
	std::unique_ptr<FCanvasRenderer>     canvasRenderer_;

	FSceneRenderer::Config config_ = {};

};
