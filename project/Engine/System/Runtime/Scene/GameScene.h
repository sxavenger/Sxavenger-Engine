#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Window/GameWindow.h>

#include "Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h"
#include "Engine/Content/Model/Model.h"
#include "Engine/Module/Transform/TransformComponent.h"
#include "Engine/Module/Camera/Camera3d.h"
#include "Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h"
#include "Engine/Asset/Asset.h"

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class
////////////////////////////////////////////////////////////////////////////////////////////
class GameScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GameScene()  = default;
	~GameScene() = default;

	void Run();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<GameWindow> mainWindow_;
	std::weak_ptr<GameWindow> subWindow_;

	bool renderWindowSwitch_ = true;

	std::unique_ptr<Model> model_;
	TransformComponent transform_;

	std::unique_ptr<Camera3d> camera_;

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;

	std::unique_ptr<SxavGraphicsFrame> g_;

	std::unique_ptr<DxObject::DimensionBuffer<Matrix4x4>> matrix_;

	Asset asset_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SystemInit();
	void Init();

	void Update();

	void Draw();

	void Term();
};