#include "MaterialStorage.h"

//=========================================================================================
// static const variable
//=========================================================================================

const std::filesystem::path MaterialStorage::kJsonDirectory = "assets/json/material";

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

Material* MaterialStorage::GetMaterial(const std::string& name) {
	if (!storage_.contains(name)) {
		storage_[name] = std::make_unique<Material>();
		storage_[name]->CreateBuffer();
	}

	return storage_[name].get();
}

void MaterialStorage::OutputJson() const {
	for (const auto& [name, material] : storage_) {
		json root = material->OutputJson();
		JsonAdapter::WriteToJson(kJsonDirectory / (name + ".json"), root);
	}
}

void MaterialStorage::InputJson() {
	for (const auto& entry : std::filesystem::directory_iterator(kJsonDirectory)) {
		json root        = JsonAdapter::LoadFromJson(entry.path().string());
		std::string name = entry.path().stem().generic_string(); 

		GetMaterial(name)->InputJson(root);
	}
}

MaterialStorage* MaterialStorage::GetInstance() {
	static MaterialStorage instance;
	return &instance;
}
