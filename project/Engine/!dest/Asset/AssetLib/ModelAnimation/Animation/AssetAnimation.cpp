#include "AssetAnimation.h"

//=========================================================================================
// static variables
//=========================================================================================

const std::unordered_set<std::filesystem::path> AssetAnimation::extension_ = {
	".gltf", ".fbx"
};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetAnimation class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetAnimation::SetAsyncTask(uint32_t assimpOption) {

	assimpOption_ = assimpOption;

	function_ = [this](_MAYBE_UNUSED const AsyncThread* const thread) {
		AnimationGroup::Load(filepath_, assimpOption_);
		isLoaded_ = true;
	};
}

void AssetAnimation::SetInspectorImGui() {
}

bool AssetAnimation::CheckExtension(const std::filesystem::path& extension) {
	return extension_.contains(extension);
}
