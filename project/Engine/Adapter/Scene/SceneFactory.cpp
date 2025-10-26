#include "SceneFactory.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneFactory class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& name) const {
	if (!factory_.contains(name)) {
		throw std::runtime_error("SceneFactory::CreateScene: Scene not found: " + name);
	}

	return factory_.at(name)();
}
