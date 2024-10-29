#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// console
#include <Engine/Console/SystemConsole.h>

// c++
#include <memory>
#include <format>

//* Game
#include <Game/Teapot.h>
#include <Game/Plane.h>
#include <Game/Particle.h>
#include <Game/AtmosphericScattering.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class
////////////////////////////////////////////////////////////////////////////////////////////
class GameScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Run();

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::unique_ptr<Teapot> teapot_;
	std::unique_ptr<Plane> plane_;

	std::unique_ptr<AtmosphericScattering> atmosphericScattering_;

	std::unique_ptr<ParitcleCollection> particleCollection_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void Draw();

};