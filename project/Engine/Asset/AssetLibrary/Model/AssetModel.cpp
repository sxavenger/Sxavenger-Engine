#include "AssetModel.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//=========================================================================================
// static variables
//=========================================================================================

const std::unordered_set<std::filesystem::path> AssetModel::extension_ = {
	".obj", ".gltf", ".fbx"
};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetModel class
////////////////////////////////////////////////////////////////////////////////////////////

void AssetModel::AsyncLoad(uint32_t assimpOption) {

	assimpOption_ = assimpOption;

	task_ = [&](_MAYBE_UNUSED const Thread* const thread) {
		Model::Load(filepath_, thread, assimpOption_);
	};
}

void AssetModel::SetInspectorImGui() {
}

bool AssetModel::CheckExtension(const std::filesystem::path& extension) {
	return extension_.contains(extension);
}
