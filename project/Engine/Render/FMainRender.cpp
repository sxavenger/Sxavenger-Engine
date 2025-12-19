#include "FMainRender.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "Pass/FRenderPassDeferredBase.h"
#include "Pass/FRenderPassDeferredLighting.h"
#include "Pass/FRenderPassAntiAliasing.h"
#include "Pass/FRenderPassForwardTransparent.h"
#include "Pass/FRenderPassPostProcess.h"
#include "Pass/FRenderPassTonemap.h"
#include "Pass/FRenderPassCanvas.h"

//* engine
#include <Engine/System/Configuration/Configuration.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FMainRender class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FMainRender::Init() {
	buffer_ = std::make_unique<FRenderTargetBuffer>();
	buffer_->Create(Configuration::GetConfig().resolution);

	scene_ = std::make_unique<FScene>();
	scene_->Init();

	context_ = std::make_unique<FRenderPassContext>();
	context_->Add<FRenderPassDeferredBase>();
	context_->Add<FRenderPassDeferredLighting>();
	context_->Add<FRenderPassForwardTransparent>();
	context_->Add<FRenderPassAntiAliasing>();
	context_->Add<FRenderPassPostProcess>();
	context_->Add<FRenderPassTonemap>();
	context_->Add<FRenderPassCanvas>();

	
}

void FMainRender::Term() {
	buffer_.reset();
	scene_.reset();
	context_.reset();
}

void FMainRender::Render(const DirectXQueueContext* context, DirectXWindowContext* window) {

	FBaseRenderPass::Config config = config_;
	config.buffer     = buffer_.get();
	config.colorSpace = window->GetColorSpace();
	config.scene      = scene_.get();

	context_->Render(context, config);

}

void FMainRender::PresentMain(const DirectXQueueContext* context) {
	context->BeginEvent(L"Present Main");

	FPresenter::Present(
		context, Configuration::GetConfig().resolution,
		buffer_->GetGBuffer(FMainGBuffer::Layout::Scene)->GetGPUHandleSRV()
	);

	FPresenter::Present(
		context, Configuration::GetConfig().resolution,
		buffer_->GetGBuffer(FMainGBuffer::Layout::Canvas)->GetGPUHandleSRV()
	);

	context->EndEvent();
}

FMainRender* FMainRender::GetInstance() {
	static FMainRender instance;
	return &instance;
}
