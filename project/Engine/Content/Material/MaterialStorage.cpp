#include "MaterialStorage.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

Material* MaterialStorage::CreateMaterial(const std::string& name) {
	storage_[name] = std::make_unique<Material>();
	storage_[name]->CreateBuffer();
	return storage_[name].get();
}

MaterialStorage* MaterialStorage::GetInstance() {
	static MaterialStorage instance;
	return &instance;
}
