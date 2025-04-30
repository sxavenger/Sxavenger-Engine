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

	std::unique_ptr<CustomReflectionComputePipeline> pipeline_;
	std::unique_ptr<CustomReflectionComputePipeline> pipeline2_;

	struct Data {
		uint32_t value;
	};

	ComPtr<ID3D12Resource> append_;
	DxObject::Descriptor descriptor_;

	ComPtr<ID3D12Resource> counter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

};
