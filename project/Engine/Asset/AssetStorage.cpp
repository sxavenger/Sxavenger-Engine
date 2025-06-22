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

AssetStorage* AssetStorage::GetInstance() {
	static AssetStorage instance;
	return &instance;
}
