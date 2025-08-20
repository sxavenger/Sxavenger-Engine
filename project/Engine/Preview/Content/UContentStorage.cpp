#include "UContentStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// UContentStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<UBaseContent> UContentStorage::GetContent(const std::type_info* type, const std::filesystem::path& filepath) const {
	if (!Contains(type, filepath)) {
		Exception::Assert(false, "content is not registered in storage.");
	}

	return storage_.at(type).At(filepath);
}

const std::type_info* UContentStorage::GetType(const std::filesystem::path& filepath) const {
	for (const auto& [type, contents] : storage_) {
		if (contents.Contains(filepath)) {
			return type;
		}
	}
	// TODO: registryを使いO(n)を避ける

	return nullptr;
}

UContentStorage* UContentStorage::GetInstance() {
	static UContentStorage instance;
	return &instance;
}

bool UContentStorage::Contains(const std::type_info* type, const std::filesystem::path& filepath) const {
	return storage_.contains(type) && storage_.at(type).Contains(filepath);
}
