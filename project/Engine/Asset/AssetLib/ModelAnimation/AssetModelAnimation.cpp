#include "AssetModelAnimation.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AssetAnimation class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<AssetModel> AssetModelAnimation::CreateModel() {
	modelAnimation_.first = std::make_shared<AssetModel>();
	modelAnimation_.first->SetFilepath(filepath_);
	return modelAnimation_.first;
}

std::shared_ptr<AssetModel> AssetModelAnimation::TryCreateModel() {
	if (modelAnimation_.first == nullptr) {
		modelAnimation_.first = std::make_shared<AssetModel>();
		modelAnimation_.first->SetFilepath(filepath_);
	}

	return modelAnimation_.first;
}

std::shared_ptr<AssetAnimation> AssetModelAnimation::CreateAnimation() {
	modelAnimation_.second = std::make_shared<AssetAnimation>();
	modelAnimation_.second->SetFilepath(filepath_);
	return modelAnimation_.second;
}

std::shared_ptr<AssetAnimation> AssetModelAnimation::TryCreateAnimation() {
	if (modelAnimation_.first == nullptr) {
		modelAnimation_.second = std::make_shared<AssetAnimation>();
		modelAnimation_.second->SetFilepath(filepath_);
	}

	return modelAnimation_.second;
}

void AssetModelAnimation::SetInspectorImGui() {
}

bool AssetModelAnimation::CheckExtension(const std::filesystem::path& extension) {
	return AssetModel::CheckExtension(extension) || AssetAnimation::CheckExtension(extension);
}
