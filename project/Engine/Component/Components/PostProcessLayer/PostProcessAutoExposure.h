#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* post process
#include "BasePostProcess.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxReadbackDimensionBuffer.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessAutoExposure class
////////////////////////////////////////////////////////////////////////////////////////////
class PostProcessAutoExposure
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

		void SetImGuiCommand();

		//=========================================================================================
		// public variables
		//=========================================================================================

		float minLogLuminance;
		float maxLogLuminance;
		float timeCoeff;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) override;

	void ShowInspectorImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

	static inline const uint32_t kGroupCount_ = 256;
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<uint32_t>> histgram_;
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<uint32_t>> histgramShared_;
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<float>>    averageLuminance_;

	//* debug
	std::unique_ptr<DxObject::ReadbackDimensionBuffer<uint32_t>> debugHistgram_;
	std::unique_ptr<DxObject::ReadbackDimensionBuffer<float>>    debugAverageLuminance_;

};
