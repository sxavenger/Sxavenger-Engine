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

void ContentScene::Load(MAYBE_UNUSED const DirectXQueueContext* context) {
	bool success = JsonHandler::LoadFromJson(BaseContent::GetFilepath(), data_);
	StreamLogger::AssertA(success, "content scene load failed. filepath: " + BaseContent::GetFilepath().generic_string());

	BaseContent::SetComplete(); //!< 読み込み完了
}
