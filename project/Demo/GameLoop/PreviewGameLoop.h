#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/GameLoop/GameLoop.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Render/FRenderTargetTextures.h>
#include <Engine/Render/FPresenter.h>
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

	FPresenter presenter_;
	FTexture texture_;

	AssetObserver<AssetTexture> environment_;

	std::unique_ptr<CustomReflectionComputePipeline> pipeline_;

	ControllableCameraActor actor_;

	std::unique_ptr<DxObject::DimensionBuffer<std::pair<uint32_t, float>>> parameter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitGame();

	void TermGame();

	void UpdateGame();

	void DrawGame();

};
