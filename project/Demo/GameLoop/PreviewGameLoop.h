#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/GameLoop/GameLoop.h>
#include <Engine/System/DirectX/DxObject/DxAppendConsumeBuffer.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Render/FPresenter.h>
#include <Engine/Render/Common/FTexture.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>
#include <Engine/Adapter/Actor/ControllableCameraActor.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// PreviewGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class PreviewGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(GameLoop::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<DirectXWindowContext> main_;

	//* preview *//

	FTexture texture_;

	CustomReflectionComputePipeline pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

};
