#include "CustomGraphicsPipeline.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Preview/Content/UContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Base CustomGraphicsPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////


void BaseCustomGraphicsPipeline::SetContent(const std::shared_ptr<UContentBlob>& blob, DxObject::GraphicsShaderType type) {
	contents_[static_cast<uint8_t>(type)].emplace() = blob;
}

void BaseCustomGraphicsPipeline::CreateContent(const std::filesystem::path& filepath, DxObject::GraphicsShaderType type) {
	std::shared_ptr<UContentBlob> blob = sUContentStorage->Import<UContentBlob>(filepath, ToProfile(type));
	SetContent(blob, type);
}

void BaseCustomGraphicsPipeline::ClearContent() {
	std::generate(contents_.begin(), contents_.end(), []() { return std::nullopt; });
}

void BaseCustomGraphicsPipeline::ReloadContent() {
	for (uint8_t i = 0; i < contents_.size(); ++i) {
		if (contents_[i].has_value()) {
			(*contents_[i]).Reload();
		}
	}
}

bool BaseCustomGraphicsPipeline::CheckAsset() const {
	return std::any_of(contents_.begin(), contents_.end(), [](const std::optional<UContentObserver<UContentBlob>>& asset) {
		return asset.has_value() && (*asset).IsExpired();
	});
}

////////////////////////////////////////////////////////////////////////////////////////////
// CustomGraphicsPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CustomGraphicsPipeline::RegisterBlob() {
	for (uint8_t i = 0; i < contents_.size(); ++i) {
		if (contents_[i].has_value()) {
			SetBlob((*contents_[i]).WaitGet()->GetBlob(), static_cast<GraphicsShaderType>(i));

		} else {
			blobs_[i] = std::nullopt;
		}
	}
}

void CustomGraphicsPipeline::ReloadAndSetPipeline(const DirectXQueueContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) {
	CheckAndReload();
	GraphicsPipelineState::SetPipeline(context->GetDxCommand(), viewport, rect);
}

void CustomGraphicsPipeline::ReloadAndSetPipeline(const DirectXQueueContext* context, const Vector2ui& windowSize) {
	CheckAndReload();
	GraphicsPipelineState::SetPipeline(context->GetDxCommand(), windowSize);
}

void CustomGraphicsPipeline::CheckAndReload() {
	if (CheckAsset()) {
		RegisterBlob();
		GraphicsPipelineState::CreateDirectXPipeline(SxavengerSystem::GetDxDevice());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// CustomReflectionGraphicsPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CustomReflectionGraphicsPipeline::RegisterBlob() {
	for (uint8_t i = 0; i < contents_.size(); ++i) {
		if (contents_[i].has_value()) {
			SetBlob((*contents_[i]).WaitAcquire()->GetBlob(), static_cast<GraphicsShaderType>(i));

		} else {
			blobs_[i] = std::nullopt;
		}
	}
}

void CustomReflectionGraphicsPipeline::ReloadAndSetPipeline(const DirectXQueueContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) {
	CheckAndReload();
	GraphicsPipelineState::SetPipeline(context->GetDxCommand(), viewport, rect);
}

void CustomReflectionGraphicsPipeline::ReloadAndSetPipeline(const DirectXQueueContext* context, const Vector2ui& windowSize) {
	CheckAndReload();
	GraphicsPipelineState::SetPipeline(context->GetDxCommand(), windowSize);
}

void CustomReflectionGraphicsPipeline::CheckAndReload() {
	if (CheckAsset()) {
		RegisterBlob();
		ReflectionGraphicsPipelineState::ReflectionRootSignature(SxavengerSystem::GetDxDevice());
		ReflectionGraphicsPipelineState::CreateDirectXPipeline(SxavengerSystem::GetDxDevice());
	}
}


