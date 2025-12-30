#include "ContentBehaviour.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentBehaviour class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentBehaviour::AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) {
	BaseContent::CheckExist();

	Load(BaseContent::GetFilepath());
}

void ContentBehaviour::AttachUuid() {
	BaseContent::CheckExist();
}

void ContentBehaviour::Load(const std::filesystem::path& filepath) {
	bool result = JsonHandler::LoadFromJson(filepath, data_);
	StreamLogger::AssertA(result, "content scene load failed. filepath: " + filepath.generic_string());
}
