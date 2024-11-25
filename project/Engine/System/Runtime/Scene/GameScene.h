#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Window/GameWindow.h>

#include "Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h"
#include "Engine/System/Runtime/Performance/DeltaTimePoint.h"
#include "Engine/Content/InputAssembler/InputAssembler.h"
#include "Engine/Module/Transform/TransformComponent.h"
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
	std::weak_ptr<GameWindow> subWindow_;

	InputAssembler<Vector4f> input_;
	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> state_;

	TransformComponent transform_;

	bool renderWindowSwitch_ = true;

	Performance p_;
	DeltaTimePoint<SecondsUnit::s> ds_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Init();

	void Update();

	void Draw();

	void Term();
};