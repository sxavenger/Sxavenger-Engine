#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Render/FPresenter.h>
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/Scene/FSceneRenderer.h>
#include <Engine/Asset/Observer/AssetObserver.h>

#include <Engine/Adapter/Actor/AtmosphereActor.h>
#include <Engine/Adapter/Actor/SkyLightActor.h>
#include <Engine/Adapter/Actor/ControllableCameraActor.h>

//* demo
#include <Demo/Entity/Player.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class BetaSystemGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	void Init(GameLoop::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<GameWindow> main_;

	//std::unique_ptr<MonoBehaviour> camera_;
	std::unique_ptr<ControllableCameraActor> camera_;

	std::unique_ptr<MonoBehaviour> lightA_;
	std::unique_ptr<MonoBehaviour> lightB_;

	std::unique_ptr<AtmosphereActor> atmosphere_;

	std::unique_ptr<SkyLightActor> skylight_;
	std::unique_ptr<MonoBehaviour> offlineSkylight_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
