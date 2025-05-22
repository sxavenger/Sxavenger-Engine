#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* composite process
#include "BaseCompositeProcess.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Asset/Assets/Texture/AssetTexture.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Render/Common/FLUTTexture.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// CompositeProcessLUT class
////////////////////////////////////////////////////////////////////////////////////////////
class CompositeProcessLUT
	: public BaseCompositeProcess {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Process(const DirectXThreadContext* context, FRenderTargetTextures* textures) override;

	void ShowInspectorImGui() override;

	//* option *//

	void CreateTexture(const DirectXThreadContext* context, const AssetObserver<AssetTexture>& texture, const Vector2ui& tile);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FLUTTexture> texture_;

};
