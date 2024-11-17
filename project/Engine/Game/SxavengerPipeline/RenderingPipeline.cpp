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

	deferredPiplineDesc_.SetDSVFormat(DXGI_FORMAT_D32_FLOAT_S8X24_UINT);

	forwardPipelineDesc_ = {};
	forwardPipelineDesc_.CreateDefaultDesc();

	forwardPipelineDesc_.SetDSVFormat(DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
}

void RenderingPipeline::CreateForward() {

	auto deviceObj = Sxavenger::GetDevicesObj();

	{
		blobs_[kDefaultVS_AlbedoPS] = std::make_unique<GraphicsBlob>();
		blobs_[kDefaultVS_AlbedoPS]->Create(L"behavior/default.vs.hlsl", GRAPHICS_VERTEX);
		blobs_[kDefaultVS_AlbedoPS]->Create(L"behavior/albedo.ps.hlsl",  GRAPHICS_PIXEL);

		pipelines_[kDefaultVS_AlbedoPS] = std::make_unique<GraphicsPipeline>();

		GraphicsRootSignatureDesc desc = {};
		desc.SetCBV(0, VISIBILITY_ALL, 10);          //!< Camera
		desc.SetVirtualSRV(1, VISIBILITY_VERTEX, 0); //!< Transform
		desc.SetCBV(2, VISIBILITY_VERTEX, 0);        //!< UVTransform
		desc.SetSRV(3, VISIBILITY_PIXEL, 0);         //!< Albedo
		desc.SetSampler(MODE_WRAP, VISIBILITY_PIXEL, 0);
		desc.SetCBV(4, VISIBILITY_PIXEL, 0);         //!< Color

		pipelines_[kDefaultVS_AlbedoPS]->CreateRootSignature(deviceObj, desc);
		pipelines_[kDefaultVS_AlbedoPS]->CreatePipeline(deviceObj, blobs_[kDefaultVS_AlbedoPS].get(), forwardPipelineDesc_);
	}

	{
		blobs_[kDefaultMS_AlbedoPS] = std::make_unique<GraphicsBlob>();
		blobs_[kDefaultMS_AlbedoPS]->Create(L"behavior/default.as.hlsl", GRAPHICS_AMPLIFICATION);
		blobs_[kDefaultMS_AlbedoPS]->Create(L"behavior/default.ms.hlsl", GRAPHICS_MESH);
		blobs_[kDefaultMS_AlbedoPS]->Create(L"behavior/albedo.ps.hlsl",  GRAPHICS_PIXEL);

		pipelines_[kDefaultMS_AlbedoPS] = std::make_unique<GraphicsPipeline>();

		GraphicsRootSignatureDesc desc = {};
		desc.SetVirtualSRV(0, VISIBILITY_ALL, 10); //!< vertices
		desc.SetVirtualSRV(1, VISIBILITY_ALL, 11); //!< indices
		desc.SetVirtualSRV(2, VISIBILITY_ALL, 12); //!< meshlets
		desc.SetVirtualSRV(3, VISIBILITY_ALL, 13); //!< primitives
		desc.SetVirtualSRV(4, VISIBILITY_ALL, 14); //!< cullData
		desc.SetCBV(5, VISIBILITY_ALL, 10);        //!< camera
		desc.SetCBV(6, VISIBILITY_ALL, 11);        //!< meshInfo
		desc.SetVirtualSRV(7, VISIBILITY_ALL, 15); //!< transform
		desc.SetCBV(8, VISIBILITY_MESH, 0);        //!< uvTransform
		desc.SetSRV(9, VISIBILITY_PIXEL, 0);       //!< Albedo
		desc.SetSampler(MODE_WRAP, VISIBILITY_PIXEL, 0);
		desc.SetCBV(10, VISIBILITY_PIXEL, 0);      //!< Color

		pipelines_[kDefaultMS_AlbedoPS]->CreateRootSignature(deviceObj, desc);
		pipelines_[kDefaultMS_AlbedoPS]->CreatePipeline(deviceObj, blobs_[kDefaultMS_AlbedoPS].get(), forwardPipelineDesc_);
	}
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
		desc.SetCBV(2, VISIBILITY_VERTEX, 0);        //!< UVTransform
		desc.SetSRV(3, VISIBILITY_PIXEL, 0);         //!< Albedo
		desc.SetSampler(MODE_WRAP, VISIBILITY_PIXEL, 0);
		desc.SetCBV(4, VISIBILITY_PIXEL, 0);          //!< Color

		pipelines_[kDefaultVS_AlbedoPS_Deferred]->CreateRootSignature(deviceObj, desc);
		pipelines_[kDefaultVS_AlbedoPS_Deferred]->CreatePipeline(deviceObj, blobs_[kDefaultVS_AlbedoPS_Deferred].get(), deferredPiplineDesc_);
	}

	{
		blobs_[kDefaultMS_AlbedoPS_Deferred] = std::make_unique<GraphicsBlob>();
		blobs_[kDefaultMS_AlbedoPS_Deferred]->Create(L"behavior/default.as.hlsl",        GRAPHICS_AMPLIFICATION);
		blobs_[kDefaultMS_AlbedoPS_Deferred]->Create(L"behavior/default.ms.hlsl",        GRAPHICS_MESH);
		blobs_[kDefaultMS_AlbedoPS_Deferred]->Create(L"behavior/albedoDeferred.ps.hlsl", GRAPHICS_PIXEL);

		pipelines_[kDefaultMS_AlbedoPS_Deferred] = std::make_unique<GraphicsPipeline>();

		GraphicsRootSignatureDesc desc = {};
		desc.SetVirtualSRV(0, VISIBILITY_ALL, 10); //!< vertices
		desc.SetVirtualSRV(1, VISIBILITY_ALL, 11); //!< indices
		desc.SetVirtualSRV(2, VISIBILITY_ALL, 12); //!< meshlets
		desc.SetVirtualSRV(3, VISIBILITY_ALL, 13); //!< primitives
		desc.SetVirtualSRV(4, VISIBILITY_ALL, 14); //!< cullData
		desc.SetCBV(5, VISIBILITY_ALL, 10);        //!< camera
		desc.SetCBV(6, VISIBILITY_ALL, 11);        //!< meshInfo
		desc.SetVirtualSRV(7, VISIBILITY_ALL, 15); //!< transform
		desc.SetCBV(8, VISIBILITY_MESH, 0);        //!< uvTransform
		desc.SetSRV(9, VISIBILITY_PIXEL, 0);       //!< Albedo
		desc.SetSampler(MODE_WRAP, VISIBILITY_PIXEL, 0);
		desc.SetCBV(10, VISIBILITY_PIXEL, 0);      //!< Color

		pipelines_[kDefaultMS_AlbedoPS_Deferred]->CreateRootSignature(deviceObj, desc);
		pipelines_[kDefaultMS_AlbedoPS_Deferred]->CreatePipeline(deviceObj, blobs_[kDefaultMS_AlbedoPS_Deferred].get(), deferredPiplineDesc_);
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
