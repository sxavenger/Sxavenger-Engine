#include "UContentStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// UContentStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

UContentStorage* UContentStorage::GetInstance() {
	static UContentStorage instance;
	return &instance;
}

bool UContentStorage::Contains(const std::type_info* type, const std::filesystem::path& filepath) const {
	return storage_.contains(type) && storage_.at(type).contains(filepath);
}
