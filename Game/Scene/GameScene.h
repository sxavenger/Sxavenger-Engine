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
#include <Cube.h>

#include <SubobjectManager.h>

#include <FullScreen.h>

#include <Particle.h>

#include <DefferedRendering.h>

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

	// particle
	std::unique_ptr<Particle> particle_;

	// defferedRendering
	std::unique_ptr<DefferedRendering> defferedRendering_;

	//-----------------------------------------------------------------------------------------
	// raytracing world
	//-----------------------------------------------------------------------------------------

	//// tlas //

	//std::unique_ptr<DxrObject::TopLevelAS> tlas_;

	//// object //

	//std::unique_ptr<Ground> ground_;
	//std::unique_ptr<Player> player_;
	//std::unique_ptr<Teapot> teapot_;
	//std::unique_ptr<Cube>   cube_;

	//std::unique_ptr<SubobjectManager> subobjectManager_;

	//// constantBuffer //

	//std::unique_ptr<RayTracingCamera> camera_;
	//std::unique_ptr<RayTracingLight>  light_;

	//// drawer //

	//std::unique_ptr<FullScreen> fullscreen_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void Draw();

};