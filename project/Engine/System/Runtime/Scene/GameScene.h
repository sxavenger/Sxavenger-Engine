#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Window/GameWindow.h>

#include "Demo/ChessBoard.h"
#include "Demo/Particle/Emitter.h"
#include "Demo/DemoParticle/Smoke.h"
#include "Demo/DemoParticle/Illumination.h"

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class
////////////////////////////////////////////////////////////////////////////////////////////
class GameScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GameScene()  = default;
	~GameScene() = default;

	void Run();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<GameWindow> mainWindow_;
	std::weak_ptr<GameWindow> subWindow_;

	bool renderWindowSwitch_ = true;

	std::unique_ptr<ChessBoard> chess_;

	std::unique_ptr<Emitter> emitter_;
	std::unique_ptr<Smoke> smoke_;

	std::unique_ptr<Illmination> illmination_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SystemInit();
	void Init();

	void Update();

	void Draw();
	void DrawScreen();

	void Term();
};