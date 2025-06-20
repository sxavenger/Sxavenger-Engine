#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/GameLoop/GameLoop.h>
#include <Engine/System/Window/GameWindow.h>
#include <Engine/System/SxavengerSystem.h>

//* preview
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

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

	std::shared_ptr<GameWindow> main_;

	//* preview *//

	DxObject::ReflectionComputePipelineState pipeline_;

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor descriptorUAV_;

	Vector3ui size_ = { 512, 512, 6 };
	

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

};
