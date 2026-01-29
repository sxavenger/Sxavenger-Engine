#include "SceneFactory.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneFactory class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& name) const {
	Exception::Assert(factory_.contains(name), "scene not found. name: " + name);
	return factory_.at(name)();
}
