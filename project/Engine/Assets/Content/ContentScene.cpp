#include "ContentScene.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentScene::AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) {
	BaseContent::CheckExist();

	Load(BaseContent::GetFilepath());
}

void ContentScene::AttachUuid() {
	BaseContent::CheckExist();
}

void ContentScene::Load(const std::filesystem::path& filepath) {
	bool result = JsonHandler::LoadFromJson(filepath, data_);
	StreamLogger::AssertA(result, "content scene load failed. filepath: " + filepath.generic_string());
}
