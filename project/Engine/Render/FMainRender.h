#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FPresenter.h>
#include <Engine/Render/FRenderTargetTextures.h>
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/Scene/FSceneRenderer.h>
#include <Engine/Render/Canvas/FCanvasRenderer.h>

//* engine
#include <Engine/System/DirectX/DirectXContext.h>

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

	void Render(const DirectXThreadContext* context);

	void PresentMain(const DirectXThreadContext* context);

	//* getter *//

	FRenderTargetTextures* GetTextures() const { return textures_.get(); }

	FSceneRenderer* GetRenderer() const { return sceneRenderer_.get(); }

	FScene* GetScene() const { return scene_.get(); }

	FCanvasRenderer* GetCanvas() const { return canvasRenderer_.get(); }

	//* singleton *//

	static FMainRender* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FPresenter> presenter_;

	std::unique_ptr<FRenderTargetTextures> textures_;
	std::unique_ptr<FSceneRenderer>        sceneRenderer_;
	std::unique_ptr<FScene>                scene_;
	std::unique_ptr<FCanvasRenderer>       canvasRenderer_;

};
