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

	CineCamera* camera_ = nullptr;
	EulerTransformBuffer buffer_;

	Vector3f point_ = {};

	uint32_t subdivision = 20;
	DeltaTimePoint time_ = { 0.6f };
	DeltaTimePoint timer_;

	uint32_t i = static_cast<uint32_t>(time_.time * subdivision);


};