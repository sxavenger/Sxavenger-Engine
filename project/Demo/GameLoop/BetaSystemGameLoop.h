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
#include <Engine/Component/Components/Light/DirectionalLightComponent.h>
#include <Engine/Render/FPresenter.h>
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/Scene/FSceneRenderer.h>
#include <Engine/Asset/Observer/AssetObserver.h>

#include "Engine/Render/Scene/Environment/Atmosphere/FSkyAtmosphere.h"

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

	std::unique_ptr<MonoBehaviour> camera_;

	std::unique_ptr<MonoBehaviour> meshA_;
	std::unique_ptr<MonoBehaviour> meshB_;

	std::unique_ptr<MonoBehaviour> foundation_;

	std::unique_ptr<MonoBehaviour> light_;
	std::unique_ptr<MonoBehaviour> skylight_;

	FEnvironmentMap environmentMap_;
	FSkyAtmosphere skyAtmosphere_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
