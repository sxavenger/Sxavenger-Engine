#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/!Render/FSceneTextures.h>
#include <Engine/!Render/Presenter/FPresenter.h>

//* game
#include "Engine/Adapter/Scene/SceneController.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ActionGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class ActionGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	void Init(GameLoop::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================



	std::unique_ptr<SceneController> collection_;

	std::unique_ptr<FSceneTextures> textures_;
	std::unique_ptr<FPresenter>     presenter_;

	//=========================================================================================
	// private method
	//=========================================================================================

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

};
