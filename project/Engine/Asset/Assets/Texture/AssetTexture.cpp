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
	Texture::Load(context, filepath_);
}

void AssetTexture::ShowInspector() {
	BaseAsset::ShowInspector();

	ImGui::Image(
		Texture::GetGPUHandleSRV().ptr,
		{ static_cast<float>(Texture::GetSize().x), static_cast<float>(Texture::GetSize().y) }
	);
}
