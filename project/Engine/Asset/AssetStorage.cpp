#include "AssetStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AssetStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetStorage::Term() {
	storage_.clear();
}

std::shared_ptr<BaseAsset> AssetStorage::GetAsset(const std::type_info* type, const std::filesystem::path& filepath) const {
	Exception::Assert(storage_.contains(type), "type is not registered in storage.");
	Exception::Assert(storage_.at(type).Contains(filepath), "filepath is not registered in storage.");

	return storage_.at(type).At(filepath);
}

bool AssetStorage::Contains(const std::filesystem::path& filepath) const {
	return registry_.Contains(filepath);
}

const std::type_info* AssetStorage::GetType(const std::filesystem::path& filepath) const {
	if (!registry_.Contains(filepath)) {
		return nullptr;
	}

	return registry_.At(filepath);
}

void AssetStorage::ImportExtension(const std::filesystem::path& filepath) {
	if (!std::filesystem::exists(filepath)) {
		Logger::WarningRuntime("warning | asset extension is not exists.", filepath.generic_string());
		return;
	}

	const auto& extension = filepath.extension();

	if (!extensions_.contains(extension)) {
		Logger::WarningRuntime("warning | asset extension not registered.", filepath.generic_string());
		return;
	}

	const auto& [type, function] = extensions_.at(extension);

	// assetの作成
	std::shared_ptr<BaseAsset> asset = function();
	asset->SetFilepath(filepath);
	asset->SetParam({});

	// storageに登録
	storage_[type][filepath] = asset;
	SxavengerSystem::PushTask(asset->GetAsyncExecution(), asset);

	// registryに登録
	registry_.Emplace(filepath, type);
}

AssetStorage* AssetStorage::GetInstance() {
	static AssetStorage instance;
	return &instance;
}
