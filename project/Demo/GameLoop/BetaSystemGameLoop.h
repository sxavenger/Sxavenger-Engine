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
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/Scene/FSceneRenderer.h>
#include <Engine/Editor/Editors/OutlinerEditor.h>
#include <Engine/Asset/Observer/AssetObserver.h>

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

	std::unique_ptr<MonoBehaviour> mesh_;

	std::unique_ptr<MonoBehaviour> camera_;

	std::unique_ptr<MonoBehaviour> light_;

	std::unique_ptr<Attribute> attribute_;

	static const uint32_t kTreeCount = 12;
	std::array<std::unique_ptr<MonoBehaviour>, kTreeCount> trees_;

	std::unique_ptr<MonoBehaviour> tile_;

	std::unique_ptr<MonoBehaviour> cube_;
	AssetObserver<AssetAnimator> animator_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
