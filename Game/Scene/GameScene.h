#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// console
#include <Console.h>

// c++
#include <memory>

// engine
#include <TextureManager.h>

// RayTracing
#include <RayTracingCamera.h>
#include <RayTracingLight.h>
#include <Ground.h>
#include <Teapot.h>
#include <Player.h>
#include <Cube.h>
#include <SubobjectManager.h>
//!< UNORM to SRGB
#include <FullScreen.h>

#include <Light.h>

#include <AnimationCube.h>
#include <AnimationHuman.h>

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

	std::unique_ptr<Light> light_;
	
	std::unique_ptr<AnimationCube> cube_;
	std::unique_ptr<AnimationHuman> human_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void Draw();

};