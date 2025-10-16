#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/GameLoop/GameLoop.h>
#include <Engine/System/SxavengerSystem.h>

#include <Engine/System/DirectX/DxObject/DxResourceDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxConstantBuffer.h>
#include <Engine/Render/Common/FBaseTexture.h>

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
	std::shared_ptr<DirectXWindowContext> sub1_;

	//* preview *//

	DxObject::ReflectionComputePipelineState compute_;

	FBaseTexture texture_;

	DxObject::ConstantBuffer<Color4f> constant_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitGame();

	void TermEngine();
	void TermGame();

	void BeginEngine();

	void UpdateGame();
	void UpdateEngine();

	void DrawGame();

	void EndEngine();

};
