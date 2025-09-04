#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Render/FPresenter.h>
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/Scene/FSceneRenderer.h>

#include <Engine/Adapter/Actor/AtmosphereActor.h>
#include <Engine/Adapter/Actor/SkyLightActor.h>
#include <Engine/Adapter/Actor/ControllableCameraActor.h>
#include <Engine/Adapter/Actor/ParameterActor.h>

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

	std::shared_ptr<DirectXWindowContext> main_;

	std::unique_ptr<ControllableCameraActor> camera_;

	std::unique_ptr<AtmosphereActor> atmosphere_;

	std::unique_ptr<SkyLightActor> skylight_;
	std::unique_ptr<MonoBehaviour> offlineSkylight_;

	std::unique_ptr<Player> player_;

	//std::unique_ptr<LeadParticle> leadParticle_;

	std::unique_ptr<MonoBehaviour> behaviour_;

	std::unique_ptr<ParameterActor> parameter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
