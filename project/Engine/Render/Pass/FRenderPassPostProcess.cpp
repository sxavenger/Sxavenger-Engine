#include "FRenderPassPostProcess.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/ComponentStorage.h>
#include <Engine/Component/Components/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Component/Entity/MonoBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassPostProcess class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassPostProcess::Render(const DirectXQueueContext* context, const Config& config) {

	if (!config.isEnablePostProcess) {
		return;
	}

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
			Logger::WarningRuntime("warning | [FRenderPassPostProcess]::ProcessPostProcessPass [Volume]", "PostProcessLayerComponent [Volume] has no transform.");
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

}
