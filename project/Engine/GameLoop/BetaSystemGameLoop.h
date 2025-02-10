#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Asset/AssetCollection.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>
#include <Engine/Content/TextureBuffer/UnorderedTexture.h>

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

	std::unique_ptr<CustomReflectionComputePipeline> pipeline_;

	std::unique_ptr<DxObject::DimensionBuffer<std::pair<Color4f, Color4f>>> buffer_;

	std::unique_ptr<UnorderedTexture> texture_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
