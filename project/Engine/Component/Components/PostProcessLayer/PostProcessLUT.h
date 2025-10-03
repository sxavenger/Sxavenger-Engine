#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* post process
#include "BasePostProcess.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Render/Common/FLUTTexture.h>
#include <Engine/Preview/Asset/UAssetTexture.h>
#include <Engine/Preview/Asset/UAssetParameter.h>

//* c++
#include <memory>

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

	void CreateTexture(const DirectXQueueContext* context, const UAssetParameter<UAssetTexture>& texture, const Vector2ui& tile);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	UAssetParameter<UAssetTexture> referenceTexture_;

	std::unique_ptr<FLUTTexture> texture_;
	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

	Vector2ui tile_ = {};

};
