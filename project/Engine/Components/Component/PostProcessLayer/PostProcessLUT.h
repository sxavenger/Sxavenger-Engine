#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* post process
#include "BasePostProcess.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxConstantBuffer.h>
#include <Engine/Assets/Asset/AssetTexture.h>
#include <Engine/Assets/Asset/AssetParameter.h>
#include <Engine/Render/Common/FLUTTexture.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLUT class
////////////////////////////////////////////////////////////////////////////////////////////
class PostProcessLUT
	: public BasePostProcess {
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

	void Process(const DirectXQueueContext* context, const ProcessInfo& info) override;

	void ShowInspectorImGui() override;

	//* option *//

	void CreateTexture(const DirectXQueueContext* context, const AssetParameter<AssetTexture>& texture, const Vector2ui& tile);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	AssetParameter<AssetTexture> referenceTexture_;

	std::unique_ptr<FLUTTexture> texture_;
	std::unique_ptr<DxObject::ConstantBuffer<Parameter>> parameter_;

	Vector2ui tile_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
