#include "FMainRender.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "Buffer/FMainBuffer.h"
#include "Pass/FRenderPassDeferredBase.h"
#include "Pass/FRenderPassDeferredDirectLighting.h"
#include "Pass/FRenderPassLightingComposite.h"
#include "Pass/FRenderPassForwardTransparent.h"
#include "Pass/FRenderPassAntiAliasing.h"
#include "Pass/FRenderPassPostProcess.h"
#include "Pass/FRenderPassTonemap.h"
#include "Pass/FRenderPassCanvas.h"
#include "FPresenter.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FMainRender class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FMainRender::Init() {

	//!< contextの設定
	context_.Insert<FRenderPassDeferredBase>();
	context_.Insert<FRenderPassDeferredDirectLighting>();
	context_.Insert<FRenderPassLightingComposite>();
	context_.Insert<FRenderPassForwardTransparent>();
	context_.Insert<FRenderPassAntiAliasing>();
	context_.Insert<FRenderPassPostProcess>();
	context_.Insert<FRenderPassTonemap>();
	context_.Insert<FRenderPassCanvas>();

	// TODO: Indirect Lightingの追加

	//!< bufferの初期化
	buffer_ = std::make_unique<FRenderTargetBuffer>();
	buffer_->Init(Configuration::GetConfig().resolution);

	//!< sceneの初期化
	scene_ = std::make_unique<FScene>();
	scene_->Init();

	//!< configの設定
	config_.name = "Main Render";
	
	config_.buffer = buffer_.get();
	config_.scene  = scene_.get();

	config_.camera = nullptr;
	config_.tag    = CameraComponent::Tag::Game; //!< GameCameraをデフォルトで描画する

	config_.option       = FRenderConfig::OptionFlag::Default;
	config_.antiAliasing = FRenderConfig::AntiAliasing::FXAA;

}

void FMainRender::Term() {
	buffer_.reset();
	scene_.reset();
}

void FMainRender::Render(const DirectXQueueContext* context, const DirectXWindowContext* window) {

	//!< configの更新
	FRenderConfig config = config_;
	config.colorSpace = window->GetColorSpace();

	//!< 描画
	context_.Render(context, config);
}

void FMainRender::Present(const DirectXQueueContext* context, const Vector2ui& resolution) {

	if (!buffer_->HasBuffer<FMainBuffer>()) {
		RuntimeLogger::LogError("[FMainRender]", "MainBuffer is not created.");
		return;
	}

	FMainBuffer* main = buffer_->GetBuffer<FMainBuffer>();

	context->BeginEvent(L"Present Main Render");

	main->GetBuffer(FMainBuffer::Layout::Scene).TransitionDefaultState(context);
	main->GetBuffer(FMainBuffer::Layout::Canvas).TransitionDefaultState(context);

	FPresenter::Present(context, resolution, main->GetBuffer(FMainBuffer::Layout::Scene).GetGPUHandleSRV());
	FPresenter::Present(context, resolution, main->GetBuffer(FMainBuffer::Layout::Canvas).GetGPUHandleSRV());

	context->EndEvent();

}

void FMainRender::Present(const DirectXQueueContext* context, const DirectXWindowContext* window) {
	Present(context, window->GetClient());
}

FMainRender* FMainRender::GetInstance() {
	static FMainRender instance;
	return &instance;
}
