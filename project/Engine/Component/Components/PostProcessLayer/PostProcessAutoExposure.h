#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* post process
#include "BasePostProcess.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessAutoExposure class
////////////////////////////////////////////////////////////////////////////////////////////
class PostProcessAutoExposure
	: public BasePostProcess {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) override;

private:
};
