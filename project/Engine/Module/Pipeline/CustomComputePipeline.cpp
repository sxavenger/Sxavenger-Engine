#include "CustomComputePipeline.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Base CustomComputePipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseCustomComputePipeline::SetAsset(const std::optional<AssetObserver<AssetBlob>>& blob) {
	asset_ = blob;
}

void BaseCustomComputePipeline::CreateAsset(const std::filesystem::path& filepath) {
	AssetObserver<AssetBlob> observer = SxavengerAsset::TryImport<AssetBlob>(filepath, CompileProfile::cs);
	SetAsset(observer);
}

void BaseCustomComputePipeline::ClearAsset() {
	asset_ = std::nullopt;
}

void BaseCustomComputePipeline::ReloadAsset() {
	if (asset_.has_value()) {
		asset_.value().Reload();
	}
}

bool BaseCustomComputePipeline::CheckAsset() const {
	return asset_.has_value() && asset_.value().IsExpired();
}

////////////////////////////////////////////////////////////////////////////////////////////
// CustomComputePipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CustomComputePipeline::RegisterBlob() {
	if (asset_.has_value()) {
		SetBlob(*asset_.value().WaitGet());

	} else {
		blob_ = std::nullopt;
	}
}

void CustomComputePipeline::ReloadAndSetPipeline(const DirectXThreadContext* context) {
	CheckAndReload();
	CustomComputePipeline::SetPipeline(context->GetDxCommand());
}

void CustomComputePipeline::CheckAndReload() {
	if (CheckAsset()) {
		RegisterBlob();
		ComputePipelineState::CreateDirectXPipeline(SxavengerSystem::GetDxDevice());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// CustomReflectionComputePipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CustomReflectionComputePipeline::RegisterBlob() {
	if (asset_.has_value()) {
		SetBlob(*asset_.value().WaitGet());

	} else {
		blob_ = std::nullopt;
	}
}

void CustomReflectionComputePipeline::ReloadAndSetPipeline(const DirectXThreadContext* context) {
	CheckAndReload();
	ComputePipelineState::SetPipeline(context->GetDxCommand());
}

void CustomReflectionComputePipeline::CheckAndReload() {
	if (CheckAsset()) {
		RegisterBlob();
		ReflectionComputePipelineState::ReflectionPipeline(SxavengerSystem::GetDxDevice());
	}
}
