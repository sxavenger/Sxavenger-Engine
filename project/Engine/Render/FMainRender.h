#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FPresenter.h>
#include <Engine/Render/FRenderTargetTextures.h>
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/Scene/FSceneRenderer.h>
#include <Engine/Render/Canvas/FCanvas.h>

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

	void PresentMain(const DirectXThreadContext* context);

	//* getter *//

	FRenderTargetTextures* GetTextures() const { return textures_.get(); }

	FSceneRenderer* GetRenderer() const { return renderer_.get(); }

	FScene* GetScene() const { return scene_.get(); }

	FCanvas* GetCanvas() const { return canvas_.get(); }

	//* singleton *//

	static FMainRender* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FPresenter> presenter_;

	std::unique_ptr<FRenderTargetTextures> textures_;
	std::unique_ptr<FSceneRenderer>        renderer_;
	std::unique_ptr<FScene>                scene_;
	std::unique_ptr<FCanvas>               canvas_;

};
