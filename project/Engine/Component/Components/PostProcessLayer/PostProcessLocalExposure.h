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
// PostProcessLocalExposure class
////////////////////////////////////////////////////////////////////////////////////////////
class PostProcessLocalExposure
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

		float aperture;     //!< 絞り [F値]
		float shutterSpeed; //!< シャッタースピード [1/sec]
		float iso;          //!< ISO感度
		float compensation; //!< 露出補正
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Process(const DirectXQueueContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) override;

	void ShowInspectorImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};
