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
	if (BaseAsset::IsLoaded()) {
		return;
	}

	BaseAsset::SetState(BaseAsset::State::Loading);

	Texture::Load(context, filepath_);
	context->ExecuteAllAllocators();

	BaseAsset::SetState(BaseAsset::State::Complete);
}

void AssetTexture::ShowInspector() {
	BaseAsset::ShowInspector();

	ImGui::Image(
		Texture::GetGPUHandleSRV().ptr,
		{ static_cast<float>(Texture::GetSize().x), static_cast<float>(Texture::GetSize().y) }
	);
}
