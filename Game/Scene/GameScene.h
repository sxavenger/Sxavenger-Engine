#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// console
#include <Console.h>

// c++
#include <memory>

// Game
#include <RayTracingCamera.h>
#include <RayTracingLight.h>
#include <Ground.h>
#include <Teapot.h>
#include <Glass.h>
#include <Bunny.h>
#include <Player.h>
#include <Room.h>
#include <Ocean.h>

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

	//-----------------------------------------------------------------------------------------
	// system
	//-----------------------------------------------------------------------------------------

	Console* console_ = Console::GetInstance();

	//-----------------------------------------------------------------------------------------
	// raytracing world
	//-----------------------------------------------------------------------------------------

	// tlas //

	std::unique_ptr<DxrObject::TopLevelAS> tlas_;

	// object //

	std::unique_ptr<Ground> ground_; 
	std::unique_ptr<Teapot> teapot_;
	std::unique_ptr<Glass>  glass_;
	std::unique_ptr<Bunny>  bunny_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Room>   room_;
	std::unique_ptr<Ocean>  ocean_;

	// constantBuffer //

	std::unique_ptr<RayTracingCamera> camera_;
	std::unique_ptr<RayTracingLight>  light_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void Draw();

};