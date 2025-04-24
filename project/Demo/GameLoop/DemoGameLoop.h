//[DemoGameLoop.h]
#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/GameLoop/GameLoop.h>
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Render/Scene/Environment/FEnvironmentMap.h>

//* demo
#include <Demo/Object/Stage.h>
#include <Demo/Entity/Player.h>
#include <Demo/Particle/SmokeParticle.h>
#include <Demo/Particle/RainParticle.h>

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

	std::shared_ptr<GameWindow> main_;

#ifdef _DEVELOPMENT
	std::shared_ptr<GameWindow> editor_;
#endif

	//* game objects *//

	std::unique_ptr<Stage> stage_;

	std::unique_ptr<Player> player_;

	std::unique_ptr<SmokeParticle> smoke_;
	std::unique_ptr<RainParticle> rain_;

	std::unique_ptr<MonoBehaviour> sampleLight_;

	FEnvironmentMap environmentMap_;
	std::unique_ptr<MonoBehaviour> skylight_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

};
