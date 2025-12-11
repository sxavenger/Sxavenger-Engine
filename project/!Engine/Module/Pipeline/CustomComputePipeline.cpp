#include "CustomComputePipeline.h"
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Preview/Content/ContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Base CustomComputePipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseCustomComputePipeline::SetContent(const std::shared_ptr<ContentBlob>& blob) {
	content_.emplace() = blob;
}

void BaseCustomComputePipeline::CreateContent(const std::filesystem::path& filepath) {
	std::shared_ptr<ContentBlob> blob = sContentStorage->Import<ContentBlob>(filepath, CompileProfile::cs);
	SetContent(blob);
}

void BaseCustomComputePipeline::ClearContent() {
	content_ = std::nullopt;
}

void BaseCustomComputePipeline::ReloadContent() {
	if (content_.has_value()) {
		content_.value().Reload();
	}
}

bool BaseCustomComputePipeline::CheckAsset() const {
	return content_.has_value() && content_.value().IsExpired();
}

////////////////////////////////////////////////////////////////////////////////////////////
// CustomComputePipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CustomComputePipeline::RegisterBlob() {
	if (content_.has_value()) {
		SetBlob(content_.value().WaitGet()->GetBlob());

	} else {
		blob_ = std::nullopt;
	}
}

void CustomComputePipeline::ReloadAndSetPipeline(const DirectXQueueContext* context) {
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
	if (content_.has_value()) {
		SetBlob(content_.value().WaitGet()->GetBlob());

	} else {
		blob_ = std::nullopt;
	}
}

void CustomReflectionComputePipeline::ReloadAndSetPipeline(const DirectXQueueContext* context) {
	CheckAndReload();
	ComputePipelineState::SetPipeline(context->GetDxCommand());
}

void CustomReflectionComputePipeline::CheckAndReload() {
	if (CheckAsset()) {
		RegisterBlob();
		ReflectionComputePipelineState::ReflectionPipeline(SxavengerSystem::GetDxDevice());
	}
}
