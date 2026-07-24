#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene
#include "BaseScene.h"
#include "SceneFactory.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <memory>
#include <deque>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SceneController class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 現在のシーンを保持し, シーンの更新と遷移を制御するクラス
class SceneController {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(std::unique_ptr<const SceneFactory>&& factory);

	void BeginState(const std::initializer_list<std::string>& names);

	//* process option *//

	void TransitionScene();

	void UpdateScene();

	void LateUpdateScene();

	//* getter *//

	bool IsEmpty() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::deque<std::unique_ptr<BaseScene>> scenes_;

	std::unique_ptr<const SceneFactory> factory_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Push(const std::string& name);

	BaseScene* GetCurrentScene() const;

	void StartScene();

};

SXAVENGER_ENGINE_NAMESPACE_END
