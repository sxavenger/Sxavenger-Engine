#include "FMainRender.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FMainRender class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FMainRender::Init() {
	buffer_         = std::make_unique<FRenderTargetBuffer>();
	sceneRenderer_  = std::make_unique<FSceneRenderer>();
	scene_          = std::make_unique<FScene>();
	canvasRenderer_ = std::make_unique<FCanvasRenderer>();

	buffer_->Create(kMainWindowSize);

	canvasRenderer_->SetTextures(buffer_.get());

	scene_->Init();
}

void FMainRender::Term() {
	buffer_.reset();
	sceneRenderer_.reset();
	scene_.reset();
	canvasRenderer_.reset();
}

void FMainRender::Render(const DirectXQueueContext* context, DirectXWindowContext* window) {

	FSceneRenderer::Config config = {};
	config.buffer     = buffer_.get();
	config.colorSpace = window->GetColorSpace();

	sceneRenderer_->Render(context, config);
	canvasRenderer_->Render(context);
}

void FMainRender::PresentMain(const DirectXQueueContext* context) {
	FPresenter::Present(
		context, kMainWindowSize,
		buffer_->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleSRV()
	);

	FPresenter::Present(
		context, kMainWindowSize,
		buffer_->GetGBuffer(FMainGBuffer::Layout::UI)->GetGPUHandleSRV()
	);
}

FMainRender* FMainRender::GetInstance() {
	static FMainRender instance;
	return &instance;
}
