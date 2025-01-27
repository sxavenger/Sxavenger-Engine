#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Asset/AssetCollection.h>

//* test !render
#include <Engine/!Render/FSceneTextures.h>
#include <Engine/!Render/Scene/FSceneRenderer.h>
#include <Engine/!Render/Scene/FScene.h>
#include <Engine/!Render/Scene/Actor/Camera/CameraActors/APivotCameraActor.h>
#include <Engine/!Render/Scene/Actor/Geometry/GeometryActors/AModelActor.h>
#include <Engine/!Render/Scene/Actor/Light/LightActors/APointLightActor.h>
#include <Engine/!Render/Scene/FPostProcessSetting.h>
#include <Engine/!Render/Scene/PostProcess/PostProcesses/FProcessLut.h>
#include <Engine/!Render/FPresenter.h>

#include <Engine/Editor/EditorComponent/AttributeComponent.h>

//* test !raytracing
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

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

	std::unique_ptr<FSceneTextures> textures_;

	std::unique_ptr<FScene>              scene_;
	std::unique_ptr<FPostProcessSetting> setting_;

	std::unique_ptr<FSceneRenderer> renderer_;

	//* camera *//

	std::unique_ptr<APivotCameraActor> camera_;

	//* geometries *//

	std::unique_ptr<AModelActor> model_;

	//* light *//

	std::unique_ptr<APointLightActor> light_;

	//* process *//

	std::unique_ptr<FProcessLut> lut_;
	std::unique_ptr<AttributeComponent> attribute_;

	//* test *//

	std::unique_ptr<DxObject::ReflectionComputePipelineState> p_;

	FPresenter presenter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
