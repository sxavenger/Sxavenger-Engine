#include "ProcessPipeline.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ProcessPipeline::Init() {
	CreateTransition();
}

void ProcessPipeline::Term() {
}

void ProcessPipeline::SetPipeline(ProcessPipelineType type) {
	pipelines_.at(type)->SetPipeline();
}

void ProcessPipeline::Dispatch(const Vector2ui& size) {
	auto commandList = Sxavenger::GetCommandList();
	commandList->Dispatch(RoundUp(size.x, threadNum_.x), RoundUp(size.y, threadNum_.y), 1);
}

void ProcessPipeline::CreateTransition() {

	{
		blobs_[kTransition_SampleLighting] = std::make_unique<CSBlob>();
		blobs_[kTransition_SampleLighting]->Create(L"sxavenger/transition/transitionSampleLighting.cs.hlsl");

		pipelines_[kTransition_SampleLighting] = std::make_unique<CSPipeline>();

		CSRootSignatureDesc desc = {};
		desc.SetSRV(0, 10); //!< Albedo
		desc.SetSRV(1, 11); //!< Normal
		desc.SetSRV(2, 12); //!< Position
		desc.SetCBV(3, 10); //!< Config
		desc.SetUAV(4, 0);  //!< Xclipse

		pipelines_[kTransition_SampleLighting]->CreatePipeline(desc, blobs_[kTransition_SampleLighting].get());
	}
}
