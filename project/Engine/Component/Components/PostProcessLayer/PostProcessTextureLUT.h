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

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessTextureLUT class
////////////////////////////////////////////////////////////////////////////////////////////
class PostProcessTextureLUT
	: public BasePostProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui tile;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) override;

	void ShowInspectorImGui() override;

	//* option *//

	void SetLUTTexture(const AssetObserver<AssetTexture>& texture, const Vector2ui& tile) {
		texture_               = texture;
		parameter_->At(0).tile = tile;
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;
	AssetObserver<AssetTexture> texture_;

};


