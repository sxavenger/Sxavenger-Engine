#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* post process
#include "BasePostProcess.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Assets/Asset/AssetTexture.h>
#include <Engine/Assets/Asset/AssetParameter.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessSketch class
////////////////////////////////////////////////////////////////////////////////////////////
class PostProcessSketch
	: public BasePostProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void SetImGuiCommand();

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2f noise_scale;

		float luminance_threshold_x;
		float luminance_x_lit; //!< [0 ~ 1]の範囲で指定する.

		float luminance_threshold_y;
		float luminance_y_lit; //!< [0 ~ 1]の範囲で指定する.

		float hatch_rotation_min;
		float hatch_rotation_max;

		float hatch_noise_threshold;
		float hatch_noise_power;

		float hatch_luminance_threshold;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Process(const DirectXQueueContext* context, const ProcessInfo& info) override;

	void ShowInspectorImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::ConstantBuffer<Parameter>> parameter_;

	AssetParameter<AssetTexture> noiseTexture_;

};

SXAVENGER_ENGINE_NAMESPACE_END
