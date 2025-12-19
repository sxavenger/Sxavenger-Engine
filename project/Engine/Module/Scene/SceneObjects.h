#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene object
#include "SceneObject.h"

//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

//* c++
#include <list>
#include <memory>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObjects class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief SceneObject管理クラス
class SceneObjects {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* container option *//

	void AddObject(BehaviourAddress&& object);

	void Clear() { objects_.clear(); }

	void Update();

	//* json option *//

	void OutputJson(const std::filesystem::path& filepath) const;

	void InputJson(const json& data);

	//* singleton *//

	static SceneObjects* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<std::unique_ptr<SceneObject>> objects_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// instance
////////////////////////////////////////////////////////////////////////////////////////////
static SceneObjects* const sSceneObjects = SceneObjects::GetInstance();

SXAVENGER_ENGINE_NAMESPACE_END
