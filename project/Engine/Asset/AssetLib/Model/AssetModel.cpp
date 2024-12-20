#include "AssetModel.h"

//=========================================================================================
// static variables
//=========================================================================================

const std::unordered_set<std::filesystem::path> AssetModel::extension_ = {
	".obj", ".gltf", ".fbx"
};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetModel class
////////////////////////////////////////////////////////////////////////////////////////////

void AssetModel::SetAsyncTask(uint32_t assimpOption) {

	assimpOption_ = assimpOption;

	function_ = [&](_MAYBE_UNUSED const AsyncThread* const thread) {
		Model::Load(filepath_, thread, assimpOption_);
	};
}

void AssetModel::SetInspectorImGui() {
}

bool AssetModel::CheckExtension(const std::filesystem::path& extension) {
	return extension_.contains(extension);
}
