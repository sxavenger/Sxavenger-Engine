#include "AssetCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AssetCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<AssetFile> AssetCollection::Import(const std::filesystem::path& filepath) {

	AssetFolder* node = &root_;

	std::filesystem::path filename;

	for (const auto& path : filepath) {
		if (path.has_extension()) {
			node->files.TryEmplace(path, ImportFile(filepath));
			filename = path;

		} else {
			node = &node->folders[path];
		}
	}

	return node->files.At(filename);
}

std::shared_ptr<AssetTexture> AssetCollection::ImportTexture(const std::filesystem::path& filepath) {
	auto asset = Import(filepath);
	return ConvertTo<AssetTexture>(asset);
}

std::shared_ptr<AssetModelAnimation> AssetCollection::ImportModelAnimation(const std::filesystem::path& filepath) {
	auto asset = Import(filepath);
	return ConvertTo<AssetModelAnimation>(asset);
}

std::shared_ptr<AssetModel> AssetCollection::ImportModel(const std::filesystem::path& filepath) {
	auto animationModel = ImportModelAnimation(filepath);
	animationModel->TryCreateModel();
	return animationModel->GetModel();
}

std::shared_ptr<AssetAnimation> AssetCollection::ImportAnimation(const std::filesystem::path& filepath) {
	auto animationModel = ImportModelAnimation(filepath);
	animationModel->TryCreateModel();
	return animationModel->GetAnimation();
}

std::shared_ptr<AssetFile> AssetCollection::ImportFile(const std::filesystem::path& filepath) {
	if (AssetTexture::CheckExtension(filepath)) {
		return CreateAssetFile<AssetTexture>(filepath);

	} else if (AssetModelAnimation::CheckExtension(filepath)) {
		return CreateAssetFile<AssetModelAnimation>(filepath);
	}

	return CreateAssetFile<AssetUnknown>(filepath);
}
