#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// console
#include <Console.h>

// c++
#include <memory>
#include <format>

#include <Light.h>

#include <AnimationCube.h>
#include <AnimationHuman.h>
#include <Skybox.h>

#include <Particle.h>

#include <DebugCamera3D.h>

//* Game
#include <DebugObjectManager.h>

#include "RayTracingDemo/RayTracingDemo.h"
#include "ObjectDemo/ObjectDemo.h"

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

	std::unique_ptr<Camera3D> gameCamera_;
	std::unique_ptr<DebugObjectManager> debugObjectManager_;

	std::unique_ptr<Particle> particle_;

	std::unique_ptr<RayTracingDemo> demo_;
	std::unique_ptr<ObjectDemo> objectDemo_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void Draw();

};