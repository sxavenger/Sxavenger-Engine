#include "ContentScene.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ContentBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentScene::AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) {
	BaseContent::CheckExist();

	Load(BaseContent::GetFilepath());
}

void ContentScene::AttachUuid() {
	BaseContent::CheckExist();
}

void ContentScene::Load(const std::filesystem::path& filepath) {
	bool result = JsonHandler::LoadFromJson(filepath, data_);
	Exception::Assert(result, "content scene load failed. filepath: " + filepath.generic_string());
}
