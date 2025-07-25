#include "AssetScene.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// Asset Scene class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AssetScene::Load(_MAYBE_UNUSED const DirectXQueueContext* context) {
	data_ = JsonHandler::LoadFromJson(BaseAsset::GetFilepath());
}

void AssetScene::ShowInspector() {
}

const json& AssetScene::GetData() const {
	BaseAsset::WaitComplete();
	return data_;
}
