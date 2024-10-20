#include "RenderingPipeline.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Game/SxavengerPipeline/SxavengerFrame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderingPipeline class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderingPipeline::Init() {
	CreateDesc();
	CreateForward();
	CreateDeferred();
	CreatePresent();
}

void RenderingPipeline::Term() {
}

void RenderingPipeline::SetPipeline(RenderingPipelineType type) const {
	pipelines_[type]->SetPipeline(Sxavenger::GetCommandList());
}

void RenderingPipeline::CreateDesc() {
	deferredPiplineDesc_ = {};
	deferredPiplineDesc_.CreateDefaultDesc();

	deferredPiplineDesc_.rtvFormats.clear();
	deferredPiplineDesc_.SetRTVFormats(SystematicRenderingFrame::kCountOfGBuffer, SystematicRenderingFrame::formats_.data());
}

void RenderingPipeline::CreateForward() {
}

void RenderingPipeline::CreateDeferred() {

	auto deviceObj = Sxavenger::GetDevicesObj();

	{
		blobs_[kDefaultVS_AlbedoPS_Deferred] = std::make_unique<GraphicsBlob>();
		blobs_[kDefaultVS_AlbedoPS_Deferred]->Create(L"behavior/default.vs.hlsl",        GRAPHICS_VERTEX);
		blobs_[kDefaultVS_AlbedoPS_Deferred]->Create(L"behavior/albedoDeferred.ps.hlsl", GRAPHICS_PIXEL);

		pipelines_[kDefaultVS_AlbedoPS_Deferred] = std::make_unique<GraphicsPipeline>();

		GraphicsRootSignatureDesc desc = {};
		desc.SetCBV(0, VISIBILITY_ALL, 10);          //!< Camera
		desc.SetVirtualSRV(1, VISIBILITY_VERTEX, 0); //!< Transform
		desc.SetSRV(2, VISIBILITY_PIXEL, 0);         //!< Albedo
		desc.SetSampler(MODE_WRAP, VISIBILITY_PIXEL, 0);

		pipelines_[kDefaultVS_AlbedoPS_Deferred]->CreateRootSignature(deviceObj, desc);
		pipelines_[kDefaultVS_AlbedoPS_Deferred]->CreatePipeline(deviceObj, blobs_[kDefaultVS_AlbedoPS_Deferred].get(), deferredPiplineDesc_);
	}
}

void RenderingPipeline::CreatePresent() {

	blobs_[kPresentToScreen] = std::make_unique<GraphicsBlob>();
	blobs_[kPresentToScreen]->Create(L"sxavenger/presentToScreen.vs.hlsl", GRAPHICS_VERTEX);
	blobs_[kPresentToScreen]->Create(L"sxavenger/presentToScreen.ps.hlsl", GRAPHICS_PIXEL);

	pipelines_[kPresentToScreen] = std::make_unique<GraphicsPipeline>();

	GraphicsRootSignatureDesc rootDesc = {};
	rootDesc.SetSRV(0, VISIBILITY_PIXEL, 0); //!< Texture
	rootDesc.SetSampler(MODE_CLAMP, VISIBILITY_PIXEL, 0);

	pipelines_[kPresentToScreen]->CreateRootSignature(Sxavenger::GetDevicesObj(), rootDesc);

	GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();

	desc.rtvFormats.clear();
	desc.SetRTVFormat(kScreenFormat);

	pipelines_[kPresentToScreen]->CreatePipeline(Sxavenger::GetDevicesObj(), blobs_[kPresentToScreen].get(), desc);

}
