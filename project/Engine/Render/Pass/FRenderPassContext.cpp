#include "FRenderPassContext.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FMainRender.h"

//* engine
#include <Engine/Component/ComponentHelper.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassContext::Render(const DirectXQueueContext* context, const FBaseRenderPass::Config& config) {

	FBaseRenderPass::Config conf = FRenderPassContext::ApplyConfig(config);

	if (conf.CheckStatus(FBaseRenderPass::Config::Status::Error_GBuffer)) {
		return; //!< configが不適格
	}

	for (const auto& pass : passes_) {
		pass->Render(context, conf);
	}
}

void FRenderPassContext::Emplace(std::unique_ptr<FBaseRenderPass>&& pass) {
	passes_.emplace_back(std::move(pass));
}

FBaseRenderPass::Config FRenderPassContext::ApplyConfig(const FBaseRenderPass::Config& _config) {
	FBaseRenderPass::Config config = _config;

	if (config.camera == nullptr) { //!< cameraが設定されていない場合, Tagのcameraを取得
		if (config.tag != CameraComponent::Tag::None) {
			config.camera = ComponentHelper::GetCameraComponent(config.tag);
		}
	}

	if (config.cullCamera == nullptr) { //!< culling用cameraが設定されていない場合, cameraと同じものを使用する
		config.cullCamera = config.camera;
	}

	if (config.scene == nullptr) { //!< sceneが設定されていない場合, main renderから取得
		config.scene = FMainRender::GetInstance()->GetScene();
	}

	config.AttachStatus(); //!< statusの更新
	return config;
}
