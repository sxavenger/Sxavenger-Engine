#include "FRenderPassPostProcess.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/Components/Component/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Components/Component/ComponentStorage.h>
#include <Engine/Components/Entity/EntityBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassPostProcess class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassPostProcess::Render(const DirectXQueueContext* context, const Config& config) {

	if (!config.option.Test(FBaseRenderPass::Config::Option::PostProcess)) {
		return;
	}

	if (config.CheckStatus(FBaseRenderPass::Config::Status::Geometry_Warning)) {
		return;
	}

	context->BeginEvent(L"RenderPass - Post Process");
	System::BeginRecordGpu(std::format("[{}] RenderPass - Post Process", magic_enum::enum_name(config.tag)));

	config.buffer->BeginPostProcess(context);

	BasePostProcess::ProcessInfo info = {};
	info.buffer = config.buffer;
	info.camera = config.camera;

	//!< Global PostProcessの処理
	sComponentStorage->ForEachActive<PostProcessLayerComponent>([&](PostProcessLayerComponent* component) {
		if (component->GetTag() != PostProcessLayerComponent::Tag::Global) {
			return; //!< Global以外のPostProcessLayerComponentは処理しない
		}

		component->Process(context, info);
	});

	// Volume PostProcessの処理
	sComponentStorage->ForEachActive<PostProcessLayerComponent>([&](PostProcessLayerComponent* component) {
		if (component->GetTag() != PostProcessLayerComponent::Tag::Volume) {
			return; //!< Global以外のPostProcessLayerComponentは処理しない
		}

		auto transform = component->GetTransform();

		if (transform == nullptr) {
			RuntimeLogger::LogWarning("[FRenderPassPostProcess]", "PostProcessLayerComponent [Volume] has no transform.");
			return; //!< Transformがない場合は処理しない
		}

		BasePostProcess::ProcessInfo parameter = info;
		parameter.weight = component->CalculateVolumeWeight(config.camera->GetPosition());

		if (parameter.weight <= 0.0f) {
			return; //!< 重みが0以下の場合は処理しない
		}

		component->Process(context, parameter);
	});

	//!< Local Post Processの処理
	if (auto component = config.camera->GetBehaviour()->GetComponent<PostProcessLayerComponent>()) {
		if (component->GetTag() == PostProcessLayerComponent::Tag::Local) {
			component->Process(context, info);
		}
	}


	config.buffer->EndPostProcess(context);

	System::EndRecordGpu();
	context->EndEvent();

}
