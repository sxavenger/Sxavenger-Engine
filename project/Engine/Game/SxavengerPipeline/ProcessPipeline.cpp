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
	CreateXclipse();
	CreateVisual();
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
		desc.SetSRV(3, 13); //!< Material
		desc.SetCBV(4, 10); //!< Config
		desc.SetCBV(5, 0);  //!< Camera
		desc.SetUAV(6, 0);  //!< Xclipse

		pipelines_[kTransition_SampleLighting]->CreatePipeline(desc, blobs_[kTransition_SampleLighting].get());
	}
}

void ProcessPipeline::CreateXclipse() {

	{
		blobs_[kXclipse_AtmoSphericScattering] = std::make_unique<CSBlob>();
		blobs_[kXclipse_AtmoSphericScattering]->Create(L"sxavenger/xclipse/xclipseAtmoSphericScattering.cs.hlsl");

		pipelines_[kXclipse_AtmoSphericScattering] = std::make_unique<CSPipeline>();

		CSRootSignatureDesc desc = {};
		desc.SetCBV(0, 10); //!< Config
		desc.SetCBV(1, 0);  //!< Camera
		desc.SetUAV(2, 0);  //!< Output

		pipelines_[kXclipse_AtmoSphericScattering]->CreatePipeline(desc, blobs_[kXclipse_AtmoSphericScattering].get());
	}

}

void ProcessPipeline::CreateVisual() {

	{
		blobs_[kVisual_Glayscale] = std::make_unique<CSBlob>();
		blobs_[kVisual_Glayscale]->Create(L"sxavenger/visual/visualGlayscale.cs.hlsl");

		pipelines_[kVisual_Glayscale] = std::make_unique<CSPipeline>();

		CSRootSignatureDesc desc = {};
		desc.SetCBV(0, 10); //!< Config
		desc.SetSRV(1, 0);  //!< Input
		desc.SetUAV(2, 0);  //!< Output

		pipelines_[kVisual_Glayscale]->CreatePipeline(desc, blobs_[kVisual_Glayscale].get());
	}

	{
		blobs_[kVisual_LUT] = std::make_unique<CSBlob>();
		blobs_[kVisual_LUT]->Create(L"sxavenger/visual/visualLUT.cs.hlsl");

		pipelines_[kVisual_LUT] = std::make_unique<CSPipeline>();

		CSRootSignatureDesc desc = {};
		desc.SetCBV(0, 10); //!< Config
		desc.SetSRV(1, 0);  //!< Input
		desc.SetUAV(2, 0);  //!< Output

		pipelines_[kVisual_LUT]->CreatePipeline(desc, blobs_[kVisual_LUT].get());
	}

	{
		blobs_[kVisual_DoF] = std::make_unique<CSBlob>();
		blobs_[kVisual_DoF]->Create(L"sxavenger/visual/visualDoF.cs.hlsl");

		pipelines_[kVisual_DoF] = std::make_unique<CSPipeline>();

		CSRootSignatureDesc desc = {};
		desc.SetCBV(0, 10); //!< Config
		desc.SetSRV(1, 0);  //!< Input
		desc.SetSRV(2, 1);  //!< Depth
		desc.SetCBV(3, 0);  //!< Camera
		desc.SetCBV(4, 1);  //!< Parameter
		desc.SetUAV(5, 0);  //!< Output

		pipelines_[kVisual_DoF]->CreatePipeline(desc, blobs_[kVisual_DoF].get());
	}

}
