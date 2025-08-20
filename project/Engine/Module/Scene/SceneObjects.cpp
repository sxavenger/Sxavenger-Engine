#include "SceneObjects.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObjects class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneObjects::AddObject(std::unique_ptr<SceneObject>&& object) {
	objects_.emplace_back(std::move(object));
}

void SceneObjects::OutputJson(const std::filesystem::path& filepath) const {
	json root = json::array();

	for (const auto& object : objects_) {
		if (object) {
			root.emplace_back(object->PerseToJson());
		}
	}

	JsonHandler::WriteToJson(filepath, root);
}

void SceneObjects::InputJson(const json& data) {
	objects_.clear();

	for (size_t i = 0; i < data.size(); ++i) {
		std::unique_ptr<SceneObject> obj = std::make_unique<SceneObject>();
		obj->InputJson(data[i]);
		objects_.emplace_back(std::move(obj));
	}
}

void SceneObjects::InputJsonFromFilepath(const std::filesystem::path& filepath) {
	filepath;
	//AssetObserver<AssetScene> scene = sAssetStorage->TryImport<AssetScene>(filepath);
	//InputJson(scene.Acquire()->GetData());
}

void SceneObjects::Update() {
	objects_.remove_if([](const std::unique_ptr<SceneObject>& obj) { return obj->IsDelete(); });
}

SceneObjects* SceneObjects::GetInstance() {
	static SceneObjects instance;
	return &instance;
}
