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

#include <Engine/Adapter/Actor/SkyLightActor.h>
#include <Engine/Adapter/Actor/ControllableCameraActor.h>
#include <Engine/Adapter/Actor/PerformanceActor.h>

#include "Engine/Render/Common/FBaseTexture.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 機能検証用GameLoopクラス	
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
	std::unique_ptr<MonoBehaviour> process_;

	std::unique_ptr<MonoBehaviour> skylight_;
	std::unique_ptr<MonoBehaviour> behaviour_;

	std::unique_ptr<PerformanceActor> performance_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
