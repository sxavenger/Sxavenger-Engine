#include "FRenderPassPostProcess.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Buffer/FMainBuffer.h"
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreProcess.h"

//* engine
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/Components/Component/PostProcessLayer/PostProcessLayerComponent.h>
#include <Engine/Components/Component/ComponentStorage.h>
#include <Engine/Components/Entity/EntityBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPassPostProcess class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderPassPostProcess::Render(const DirectXQueueContext* context, const FRenderConfig& config) {

	if (!config.option.Test(FRenderConfig::OptionFlag::PostProcess)) {
		return; //!< PostProcessが無効な場合は処理しない
	}

	if (config.HasIssue(FRenderConfig::IssueFlag::Warning_Geometry)) {
		return; //!< Geometryに問題がある場合は処理しない
	}

	if (!config.buffer->EnsureBuffer<FMainBuffer>()) {
		RuntimeLogger::LogError("[FRenderPassPostProcess]", "MainBuffer is requires.");
		return; //!< MainBufferが確保できない場合は処理しない
	}

	FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保

	FBaseRenderPass::BeginRenderPass(context, "Post Process", config);

	{ //!< Post Process Pass

		BeginPostProcessPass(context, config.buffer);

		PostProcessGlobal(context, config);

		PostProcessVolume(context, config);

		PostProcessLocal(context, config);

		EndPostProcessPass(context, config.buffer);
	}

	FBaseRenderPass::EndRenderPass(context);

}

void FRenderPassPostProcess::BeginPostProcessPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FMainBuffer* main       = buffer->GetBuffer<FMainBuffer>();
	FProcessBuffer* process = buffer->GetProcess();

	process->Import(context, &main->GetBuffer(FMainBuffer::Layout::Scene));
	//!< Scene BufferをProcess処理用に使用状態にする.

}

void FRenderPassPostProcess::EndPostProcessPass(const DirectXQueueContext* context, FRenderTargetBuffer* buffer) {

	FMainBuffer* main       = buffer->GetBuffer<FMainBuffer>();
	FProcessBuffer* process = buffer->GetProcess();

	process->Export(context, &main->GetBuffer(FMainBuffer::Layout::Scene));
	//!< Process処理が完了したScene BufferをScene Bufferに戻す.
}

void FRenderPassPostProcess::PostProcessGlobal(const DirectXQueueContext* context, const FRenderConfig& config) {

	BasePostProcess::ProcessInfo info = {};
	info.buffer = config.buffer;
	info.camera = config.camera;
	info.weight = 1.0f;

	sComponentStorage->ForEachActive<PostProcessLayerComponent>([&](PostProcessLayerComponent* component) {
		if (component->GetTag() != PostProcessLayerComponent::Tag::Global) {
			return; //!< Global以外のPostProcessLayerComponentは処理しない
		}

		component->Process(context, info);
	});

}

void FRenderPassPostProcess::PostProcessVolume(const DirectXQueueContext* context, const FRenderConfig& config) {

	BasePostProcess::ProcessInfo info = {};
	info.buffer = config.buffer;
	info.camera = config.camera;

	sComponentStorage->ForEachActive<PostProcessLayerComponent>([&](PostProcessLayerComponent* component) {
		if (component->GetTag() != PostProcessLayerComponent::Tag::Volume) {
			return; //!< Volume以外のPostProcessLayerComponentは処理しない
		}

		auto transform = component->GetTransform();
		if (transform == nullptr) {
			RuntimeLogger::LogWarning("[FRenderPassPostProcess]", "PostProcessLayerComponent [Volume] has no transform.");
			return; //!< Transformがない場合は処理しない
		}

		//!< Volume内の重みを計算する
		BasePostProcess::ProcessInfo parameter = info;
		parameter.weight = component->CalculateVolumeWeight(config.camera->GetPosition());
		
		if (info.weight <= 0.0f) {
			return; //!< 重みが0以下の場合は処理しない
		}

		component->Process(context, info);
	});

}

void FRenderPassPostProcess::PostProcessLocal(const DirectXQueueContext* context, const FRenderConfig& config) {

	BasePostProcess::ProcessInfo info = {};
	info.buffer = config.buffer;
	info.camera = config.camera;

	if (auto component = config.camera->GetBehaviour()->GetComponent<PostProcessLayerComponent>()) {
		if (component->GetTag() == PostProcessLayerComponent::Tag::Local) {
			component->Process(context, info);
		}
	}

}
