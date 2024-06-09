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
#include <Player.h>

#include <SubobjectManager.h>

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
	// raytracing world
	//-----------------------------------------------------------------------------------------

	// tlas //

	std::unique_ptr<DxrObject::TopLevelAS> tlas_;

	// object //

	std::unique_ptr<Ground> ground_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Teapot> teapot_;

	std::unique_ptr<SubobjectManager> subobjectManager_;

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