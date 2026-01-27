#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* post process
#include "BasePostProcess.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxConstantBuffer.h>
#include <Engine/System/DirectX/DxObject/DxUnorderedDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxReadbackDimensionBuffer.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

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

	void Process(const DirectXQueueContext* context, const ProcessInfo& info) override;

	void ShowInspectorImGui() override;

	//* getter *//

	const Parameter& GetParameter() const;
	Parameter& GetParameter();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::ConstantBuffer<Parameter>> parameter_;

	static inline const uint32_t kGroupCount_ = 128;
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<uint32_t>> histogram_;
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<uint32_t>> histogramShared_;
	std::unique_ptr<DxObject::UnorderedDimensionBuffer<float>>    averageLuminance_;

	//* debug
	std::unique_ptr<DxObject::ReadbackDimensionBuffer<uint32_t>> debugHistogram_;
	std::unique_ptr<DxObject::ReadbackDimensionBuffer<float>>    debugAverageLuminance_;

};

SXAVENGER_ENGINE_NAMESPACE_END
