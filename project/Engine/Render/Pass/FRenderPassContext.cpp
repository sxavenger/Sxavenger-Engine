#include "FRenderPassContext.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FMainRender.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/Components/Component/ComponentHelper.h>

//* lib
#include <Lib/Adapter/String/EncodedString.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassContext::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	FRenderConfig resolved = FRenderPassContext::ResolveConfig(config);

	if (resolved.HasIssue(FRenderConfig::IssueFlag::Error_Buffer)) {
		RuntimeLogger::LogWarning("[FRenderPassContext]", "config buffer is not set.");
		return; //!< configが不適格
	}

	//!< Contextの実行
	context->BeginEvent(std::format(L"[{}] FRenderPassContext", EncodedString::Convert(config.name)));

	config.buffer->ClearMainRenderTarget(context);

	for (const auto& pass : passes_) {
		pass->Render(context, resolved);
	}

	context->EndEvent();
}

void FRenderPassContext::Insert(std::unique_ptr<FBaseRenderPass>&& pass) {
	passes_.emplace_back(std::move(pass));
}

FRenderConfig FRenderPassContext::ResolveConfig(const FRenderConfig& config) {

	FRenderConfig resolved = config;

	if (resolved.name.empty()) {
		resolved.name = magic_enum::enum_name(resolved.tag); //!< nameが設定されていない場合, Tagの名前を使用する
	}

	if (resolved.camera == nullptr) { //!< cameraが設定されていない場合, Tagのcameraを取得
		if (resolved.tag != CameraComponent::Tag::None) {
			resolved.camera = ComponentHelper::GetCameraComponent(resolved.tag);
		}
	}

#ifdef _DEVELOPMENT
	if (resolved.cullCamera == nullptr) { //!< culling用cameraが設定されていない場合, 同じものを使用する
		resolved.cullCamera = config.camera;
	}
#else
	resolved.cullCamera = config.camera; //!< 同じものを使用する
#endif

	if (resolved.scene == nullptr) { //!< sceneが設定されていない場合, main renderから取得
		resolved.scene = FMainRender::GetInstance()->GetScene();
	}

	resolved.CheckIssue(); //!< issueの更新
	return resolved;
	
}
