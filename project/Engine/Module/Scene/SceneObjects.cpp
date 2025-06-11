#include "SceneObjects.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObjects class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneObjects::AddObject(std::unique_ptr<SceneObject>&& object) {
	objects_.emplace_back(std::move(object));
}

void SceneObjects::OutputJson() const {

	json root = json::array();

	for (const auto& object : objects_) {
		if (object) {
			root.emplace_back(object->PerseToJson());
		}
	}

	JsonHandler::WriteToJson("test.scene", root);
}

void SceneObjects::InputJson() {
	objects_.clear();
	json root = JsonHandler::LoadFromJson("test.scene");

	for (size_t i = 0; i < root.size(); ++i) {
		std::unique_ptr<SceneObject> obj = std::make_unique<SceneObject>();
		obj->InputJson(root[i]);
		objects_.emplace_back(std::move(obj));
	}
}

SceneObjects* SceneObjects::GetInstance() {
	static SceneObjects instance;
	return &instance;
}
