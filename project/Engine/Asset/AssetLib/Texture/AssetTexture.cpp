#include "AssetTexture.h"

//=========================================================================================
// static variables
//=========================================================================================

const std::unordered_set<std::filesystem::path> AssetTexture::extension_ = {
	".png", ".jpg", ".dds"
};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetTexture::Load(const DirectXThreadContext* context) {
	if (isLoaded_) {
		return;
	}

	isLoaded_ = true;
	Texture::Load(filepath_, context);
}

void AssetTexture::SetInspectorImGui() {
}

bool AssetTexture::CheckExtension(const std::filesystem::path& extension) {
	return extension_.contains(extension);
}
