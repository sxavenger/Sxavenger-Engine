#include "CustomGraphicsPipeline.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Base CustomGraphicsPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseCustomGraphicsPipeline::SetAsset(const std::optional<AssetObserver<AssetBlob>>& blob, DxObject::GraphicsShaderType type) {
	assets_[static_cast<uint8_t>(type)] = blob;
}

void BaseCustomGraphicsPipeline::CreateAsset(const std::filesystem::path& filepath, DxObject::GraphicsShaderType type) {
	// TODO: profile set
	AssetObserver<AssetBlob> observer = SxavengerAsset::TryImport<AssetBlob>(filepath);
	SetAsset(observer, type);
}

void BaseCustomGraphicsPipeline::ClearAsset() {
	for (uint8_t i = 0; i < assets_.size(); ++i) {
		assets_[i] = std::nullopt;
	}
}

void BaseCustomGraphicsPipeline::ReloadAsset() {
	for (uint8_t i = 0; i < assets_.size(); ++i) {
		if (assets_[i].has_value()) {
			assets_[i].value().Reload();
		}
	}
}

bool BaseCustomGraphicsPipeline::CheckAsset() const {
	return std::any_of(assets_.begin(), assets_.end(), [](const std::optional<AssetObserver<AssetBlob>>& asset) {
		return asset.has_value() && asset.value().CheckExpired();
	});
}

////////////////////////////////////////////////////////////////////////////////////////////
// CustomGraphicsPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CustomGraphicsPipeline::RegisterBlob() {
	for (uint8_t i = 0; i < assets_.size(); ++i) {
		if (assets_[i].has_value()) {
			SetBlob(*assets_[i].value().WaitGet(), static_cast<GraphicsShaderType>(i));

		} else {
			blobs_[i] = std::nullopt;
		}
	}
}

void CustomGraphicsPipeline::ReloadAndSetPipeline(const DirectXThreadContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) {
	CheckAndReload();
	GraphicsPipelineState::SetPipeline(context->GetDxCommand(), viewport, rect);
}

void CustomGraphicsPipeline::ReloadAndSetPipeline(const DirectXThreadContext* context, const Vector2ui& windowSize) {
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
	for (uint8_t i = 0; i < assets_.size(); ++i) {
		if (assets_[i].has_value()) {
			SetBlob(*assets_[i].value().WaitGet(), static_cast<GraphicsShaderType>(i));

		} else {
			blobs_[i] = std::nullopt;
		}
	}
}

void CustomReflectionGraphicsPipeline::ReloadAndSetPipeline(const DirectXThreadContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) {
	CheckAndReload();
	GraphicsPipelineState::SetPipeline(context->GetDxCommand(), viewport, rect);
}

void CustomReflectionGraphicsPipeline::ReloadAndSetPipeline(const DirectXThreadContext* context, const Vector2ui& windowSize) {
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


