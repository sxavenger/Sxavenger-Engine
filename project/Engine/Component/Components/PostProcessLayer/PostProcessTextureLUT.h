#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* post process
#include "BasePostProcess.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Asset/Assets/Texture/AssetTexture.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Render/Common/FLUTTexture.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessTextureLUT class
////////////////////////////////////////////////////////////////////////////////////////////
class PostProcessTextureLUT
	: public BasePostProcess {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) override;

	void ShowInspectorImGui() override;

	//* option *//

	void CreateTexture(const DirectXThreadContext* context, const AssetObserver<AssetTexture>& texture, const Vector2ui& tile);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FLUTTexture> texture_;

};


