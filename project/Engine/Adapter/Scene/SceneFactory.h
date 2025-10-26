#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene
#include "BaseScene.h"

//* c++
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneFactory class
////////////////////////////////////////////////////////////////////////////////////////////
class SceneFactory {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using SceneFunction = std::function<std::unique_ptr<BaseScene>()>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	template <SceneConcept T>
	void Register(const std::string& name);

	std::unique_ptr<BaseScene> CreateScene(const std::string& name) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<std::string, SceneFunction> factory_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// SceneFactory class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <SceneConcept T>
inline void SceneFactory::Register(const std::string& name) {
	factory_[name] = []() { return std::make_unique<T>(); };
}
