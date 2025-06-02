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

	Vector3ui size_ = { 1280, 720, 32 };
	ComPtr<ID3D12Resource> voxel_;
	DxObject::Descriptor descriptorUAV_;

	std::unique_ptr<MonoBehaviour> behaviour_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

};
