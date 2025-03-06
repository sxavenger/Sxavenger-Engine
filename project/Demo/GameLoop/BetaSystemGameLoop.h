#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Content/Material/Material.h>
#include <Engine/Component/MonoBehaviour.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>
#include <Engine/Component/Components/Light/DirectionalLightComponent.h>
#include <Engine/Render/FPresenter.h>
#include <Engine/Render/Scene/FSceneRenderer.h>
#include <Engine/Render/Scene/Actor/Camera/CameraActors/ACineCameraActor.h>

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

	std::unique_ptr<FRenderTargetTextures> textures_;
	std::unique_ptr<FSceneRenderer> renderer_;

	FPresenter presenter_;

	std::unique_ptr<MonoBehaviour> mesh_;

	std::unique_ptr<MonoBehaviour> camera_;

	std::unique_ptr<MonoBehaviour> light_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
