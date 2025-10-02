#include "UContentScene.h"

////////////////////////////////////////////////////////////////////////////////////////////
// UContentBlob class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UContentScene::AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) {
	UBaseContent::CheckExist();

	Load(UBaseContent::GetFilepath());
}

void UContentScene::AttachUuid() {
	UBaseContent::CheckExist();
}

void UContentScene::Load(const std::filesystem::path& filepath) {
	bool result = JsonHandler::LoadFromJson(filepath, data_);
	Exception::Assert(result, "content scene load failed. filepath: " + filepath.generic_string());
}
