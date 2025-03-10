#include "FMainRender.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FMainRender class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FMainRender::Init() {
	presenter_ = std::make_unique<FPresenter>();
	textures_  = std::make_unique<FRenderTargetTextures>();
	renderer_  = std::make_unique<FSceneRenderer>();
	scene_     = std::make_unique<FScene>();
	canvas_    = std::make_unique<FCanvas>();

	presenter_->Init();

	textures_->Create(kMainWindowSize);
	renderer_->SetTextures(textures_.get());

	canvas_->SetTextures(textures_.get());
}

void FMainRender::Term() {
	textures_.reset();
	renderer_.reset();
	scene_.reset();
	canvas_.reset();
}

void FMainRender::PresentMain(const DirectXThreadContext* context) {
	presenter_->Present(
		context, kMainWindowSize,
		textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->GetGPUHandleSRV()
	);
}

FMainRender* FMainRender::GetInstance() {
	static FMainRender instance;
	return &instance;
}
