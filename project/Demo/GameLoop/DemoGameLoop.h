#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/GameLoop/GameLoop.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/Adapter/Actor/PerformanceActor.h>

//* demo
#include <Demo/Object/PerspectiveCamera.h>
#include <Demo/Entity/Player.h>
#include <Demo/Object/EmissiveCubes.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
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

	//* game objects *//

	std::unique_ptr<MonoBehaviour> skylight_;

	std::unique_ptr<PerformanceActor> performance_;

	std::unique_ptr<PerspectiveCamera> camera_;
	std::unique_ptr<Player> player_;

	std::unique_ptr<EmissiveCubes> cubes_;

	std::unique_ptr<MonoBehaviour> demoText_;
	std::unique_ptr<MonoBehaviour> text_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* game logic *//

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

	//* sub methods *//

	void SetCollisionCallback();

};
