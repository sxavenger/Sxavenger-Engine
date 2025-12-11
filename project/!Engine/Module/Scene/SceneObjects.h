#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene
#include "SceneObject.h"

//* c++
#include <list>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObjects class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief SceneObject管理クラス
class SceneObjects {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void AddObject(std::unique_ptr<SceneObject>&& object);

	void Clear() { objects_.clear(); }

	//* json option *//

	void OutputJson(const std::filesystem::path& filepath) const;

	void InputJson(const json& data);

	void InputJsonFromFilepath(const std::filesystem::path& filepath);

	void Update();

	//* singleton *//

	static SceneObjects* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<std::unique_ptr<SceneObject>> objects_;

};

//=========================================================================================
// singleton
//=========================================================================================
static SceneObjects* sSceneObjects = SceneObjects::GetInstance();
