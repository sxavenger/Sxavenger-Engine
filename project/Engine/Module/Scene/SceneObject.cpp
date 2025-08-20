#include "SceneObject.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Components/Transform/TransformComponent.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneObject::Inspectable() {
	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}

json SceneObject::PerseToJson() const {
	json root = json::object();

	//* properties
	root["name"]        = name_;
	root["isRenamable"] = isRenamable_;
	root["isActive"]    = isActive_;
	root["isView"]      = isView_;

	//* components
	json& components = root["components"] = json::object();
	for (const auto& [type, component] : components_) {
		components[type->name()] = (*component)->PerseToJson();
	}

	//* parameter
	root["filepath"] = filepath_.generic_string();

	return root;
}

void SceneObject::InputJson(const json& data) {
	data;
	// todo:

	////* parameter
	//filepath_ = data["filepath"].get<std::string>();

	//if (!filepath_.empty()) {
	//	auto asset = SxavengerAsset::TryImport<AssetModel>(filepath_);
	//	asset.WaitGet()->CreateStaticNodeMeshBehaviour(this);
	//}
	//
	////* properties
	//name_        = data["name"].get<std::string>();
	//isRenamable_ = data["isRenamable"];
	//MonoBehaviour::SetActive(data["isActive"]);
	//isView_      = data["isView"];

	//

	////* components
	//for (const auto& [name, component] : data["components"].items()) {
	//	MonoBehaviour::AddComponent(name)->InputJson(component);
	//}

}
