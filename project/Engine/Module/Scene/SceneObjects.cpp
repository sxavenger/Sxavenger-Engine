#include "SceneObjects.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Components/Entity/MonoBehaviour.h>
#include <Engine/Components/Entity/BehaviourHelper.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObjects class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneObjects::AddObject(BehaviourAddress&& address) {
	objects_.emplace_back(std::make_unique<SceneObject>(std::move(address)));
}

void SceneObjects::Update() {
	objects_.remove_if([](const std::unique_ptr<SceneObject>& obj) { return obj->IsDelete(); });
}

void SceneObjects::OutputJson(const std::filesystem::path& filepath) const {
	json root = json::array();

	for (const auto& object : objects_) {
		if (object) {
			root.emplace_back(object->GetAddress()->PerseToJson());
		}
	}

	JsonHandler::WriteToJson(filepath, root);
}

void SceneObjects::InputJson(const json& data) {
	objects_.clear();

	for (size_t i = 0; i < data.size(); ++i) {
		BehaviourAddress address = BehaviourHelper::Create();
		address->InputJson(data[i]);
		objects_.emplace_back(std::make_unique<SceneObject>(std::move(address)));
	}
}

SceneObjects* SceneObjects::GetInstance() {
	static SceneObjects instance;
	return &instance;
}
