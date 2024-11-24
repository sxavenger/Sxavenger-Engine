#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "IScene.h"

//* Game
#include <Game/Teapot.h>
#include <Game/Plane.h>
#include <Game/AnimationPlayer.h>
#include <Engine/Game/Camera/CineCamera.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Title class
////////////////////////////////////////////////////////////////////////////////////////////
class Scene_Title
	: public IScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Scene_Title()           = default;
	~Scene_Title() override { Term(); }

	void Init() override;

	void Term() override;

	void Update() override;

	void Draw() override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::unique_ptr<Teapot> teapot_;
	std::unique_ptr<Plane> plane_;

	std::unique_ptr<AnimationPlayer> animationTest_;


};