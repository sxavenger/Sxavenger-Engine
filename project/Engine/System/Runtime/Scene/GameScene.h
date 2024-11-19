#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Window/GameWindow.h>

#include "Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h"
#include "Engine/Content/InputAssembler/InputAssembler.h"
#include "Lib/Geometry/Vector4.h"

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

	GameWindow* mainWindow_ = nullptr;

	InputAssembler<Vector4f> input_;
	std::unique_ptr<DxObject::GraphicsPipelineState> state_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Init();

	void Update();

	void Draw();

	void Term();
};