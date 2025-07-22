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
		float compensation;

		// todo: up, downでparameterを変える

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Process(const DirectXQueueContext* context, FRenderTargetBuffer* buffer, const CameraComponent* camera) override;

	void ShowInspectorImGui() override;

	//* getter *//

	const Parameter& GetParameter() const;
	Parameter& GetParameter();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

	static inline const uint32_t kGroupCount_ = 128;
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<uint32_t>> histgram_;
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<uint32_t>> histgramShared_;
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<float>>    averageLuminance_;

	//* debug
	std::unique_ptr<DxObject::ReadbackDimensionBuffer<uint32_t>> debugHistgram_;
	std::unique_ptr<DxObject::ReadbackDimensionBuffer<float>>    debugAverageLuminance_;

};
