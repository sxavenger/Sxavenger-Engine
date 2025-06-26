#include "AssetTexture.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AssetTexture::Load(const DirectXThreadContext* context) {

	Texture::Encoding encoding = Texture::Encoding::Lightness; //!< default encoding

	if (param_.has_value()) {
		encoding = std::any_cast<Texture::Encoding>(param_);
	}

	Texture::Load(context, filepath_, encoding);
}

void AssetTexture::ShowInspector() {
	BaseAsset::ShowInspector();

	ImGui::Image(
		Texture::GetGPUHandleSRV().ptr,
		{ static_cast<float>(Texture::GetSize().x), static_cast<float>(Texture::GetSize().y) }
	);
}
