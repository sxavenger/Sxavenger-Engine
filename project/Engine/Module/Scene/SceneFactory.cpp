#include "SceneFactory.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneFactory class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& name) const {
	StreamLogger::AssertA(factory_.contains(name), "scene not found. name: " + name);
	return factory_.at(name)();
}
