#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/GameLoop/GameLoop.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/Adapter/Scene/SceneFactory.h>
#include <Engine/Adapter/Scene/SceneController.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Demo用GameLoop実装クラス.
class DemoGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(GameLoop::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<DirectXWindowContext> main_;

	std::unique_ptr<SceneController> controller_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* game logic *//

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

};
