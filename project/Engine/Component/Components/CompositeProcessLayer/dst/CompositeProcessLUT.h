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

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter class
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
		float intensity;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Process(const DirectXQueueContext* context, FRenderTargetTextures* textures) override;

	void ShowInspectorImGui() override;

	//* option *//

	void CreateTexture(const DirectXQueueContext* context, const AssetObserver<AssetTexture>& texture, const Vector2ui& tile);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FLUTTexture> texture_;
	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
