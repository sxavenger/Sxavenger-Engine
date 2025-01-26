#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/!Render/FSceneTextures.h>
#include <Engine/!Render/Scene/FScene.h>
#include <Engine/!Render/Scene/FSceneRenderer.h>

#include <Engine/!Render/Scene/Actor/Camera/CameraActors/ACineCameraActor.h>

#include <Engine/!Render/Scene/Actor/Geometry/GeometryActors/AModelActor.h>
#include <Engine/!Render/Scene/Actor/Geometry/GeometryActors/AModelAnimationActor.h>

#include <Engine/!Render/Scene/Actor/Light/LightActors/ADirectionalLightActor.h>
#include <Engine/!Render/Scene/Actor/Light/LightActors/APointLightActor.h>
#include <Engine/!Render/Scene/Actor/Light/LightActors/ASpotLightActor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MainGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class MainGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	void Init(GameLoop::Context* context);

	void Term();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<GameWindow> main_;

	//* render *//

	std::unique_ptr<FSceneTextures> textures_;

	std::unique_ptr<FScene>         scene_;
	std::unique_ptr<FSceneRenderer> renderer_;

	//* camera *//

	std::unique_ptr<ACineCameraActor> camera_;

	//* geometries *//

	std::unique_ptr<AModelActor> sphere_;
	std::unique_ptr<AModelActor> plane1_;
	std::unique_ptr<AModelActor> plane2_;
	std::unique_ptr<AModelActor> chessboard_;

	//* light *//

	std::array<std::unique_ptr<ADirectionalLightActor>, 2> directionals_;
	std::array<std::unique_ptr<APointLightActor>, 2>       points_;
	std::array<std::unique_ptr<ASpotLightActor>, 2>        spots_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
