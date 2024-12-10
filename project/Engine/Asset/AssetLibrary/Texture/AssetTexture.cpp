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
	if (isLoad_) {
		return;
	}

	Texture::Load(filepath_, context);
	isLoad_ = true;
}

void AssetTexture::SetInspectorImGui() {
}

bool AssetTexture::CheckExtension(const std::filesystem::path& extension) {
	return extension_.contains(extension);
}
