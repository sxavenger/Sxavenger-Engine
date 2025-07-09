#include "FMainRender.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FMainRender class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FMainRender::Init() {
	presenter_      = std::make_unique<FPresenter>();
	textures_       = std::make_unique<FRenderTargetTextures>();
	sceneRenderer_  = std::make_unique<FSceneRenderer>();
	scene_          = std::make_unique<FScene>();
	canvasRenderer_ = std::make_unique<FCanvasRenderer>();

	presenter_->Init();
	textures_->Create(kMainWindowSize);

	sceneRenderer_->SetTextures(textures_.get());
	canvasRenderer_->SetTextures(textures_.get());

	scene_->Init();
}

void FMainRender::Term() {
	textures_.reset();
	sceneRenderer_.reset();
	scene_.reset();
	canvasRenderer_.reset();
}

void FMainRender::Render(const DirectXThreadContext* context) {
	sceneRenderer_->Render(context);
	canvasRenderer_->Render(context);
}

void FMainRender::PresentMain(const DirectXThreadContext* context) {
	presenter_->Present(
		context, kMainWindowSize,
		textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->GetGPUHandleSRV()
	);

	presenter_->Present(
		context, kMainWindowSize,
		textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::UI)->GetGPUHandleSRV()
	);
}

FMainRender* FMainRender::GetInstance() {
	static FMainRender instance;
	return &instance;
}
