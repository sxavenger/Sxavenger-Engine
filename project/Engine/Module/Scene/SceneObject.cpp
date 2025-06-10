#include "SceneObject.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Asset/Observer/AssetObserver.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneObject::CreateMeshComponent(const std::filesystem::path& filepath) {
	auto asset = SxavengerAsset::TryImport<AssetModel>(filepath);
	asset.WaitGet()->CreateStaticNodeMeshBehaviour(this);
}

json SceneObject::PerseToJson() const {
	json root = json::object();

	//* properties
	root["name"]        = name_;
	root["isRenamable"] = isRenamable_;
	root["isActive"]    = isActive_;
	root["isView"]      = isView_;

	//* components
	json& components = root["components"] = json::array();
	for (const auto& [type, component] : components_) {
		components.emplace_back((*component)->PerseToJson());
	}

	//* parameter
	root["filepath"] = filepath_.generic_string();

	return root;
}

void SceneObject::InputJson(const json& data) {

	//* properties
	name_        = data["name"].get<std::string>();
	isRenamable_ = data["isRenamable"];
	isActive_    = data["isActive"];
	isView_      = data["isView"];

	//* components
	// todo:

	//* parameter
	filepath_ = data["filepath"].get<std::string>();

}
