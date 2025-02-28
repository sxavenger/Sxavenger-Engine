#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Render/FPresenter.h>
#include <Engine/Render/FSceneTextures.h>
#include <Engine/Render/Scene/FSceneRenderer.h>
#include <Engine/Render/Scene/FScene.h>
#include <Engine/Render/Scene/Actor/Camera/CameraActors/ACineCameraActor.h>
#include <Engine/Render/Scene/Actor/Geometry/GeometryActors/AModelActor.h>
#include <Engine/Render/Scene/Actor/Geometry/GeometryActors/AModelInstanceActor.h>
#include <Engine/Render/Scene/Actor/Light/LightActors/ADirectionalLightActor.h>
#include <Engine/Render/Scene/FAmbientProcessSetting.h>
#include <Engine/Render/Scene/AmbientProcess/AmbientProcesses/FAmbientProcessNLAO.h>

#include "Engine/System/DirectX/DxObject/DxVectorDimensionBuffer.h"
#include "Engine/Module/Collider/Collider.h"

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

	// vvv render system vvv //

	FPresenter presenter_;

	std::unique_ptr<FSceneTextures> textures_;
	std::unique_ptr<FSceneRenderer> renderer_;

	std::unique_ptr<FScene> scene_;

	// vvv actors vvv //

	std::unique_ptr<ACineCameraActor> camera_;

	std::unique_ptr<ADirectionalLightActor> light_;

	std::unique_ptr<AModelActor> model_;

	// vvv engine debug vvv //

	std::unique_ptr<DxObject::VectorDimensionBuffer<Vector4f>> buffer_;

	std::unique_ptr<Collider> colliders_[128];

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
