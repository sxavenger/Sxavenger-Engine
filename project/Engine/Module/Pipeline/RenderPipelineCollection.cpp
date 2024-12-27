#include "RenderPipelineCollection.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Config/SxavengerDirectory.h>
#include <Engine/Module/SxavengerGraphics/SystematicRenderFrame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPipelineCollection class
////////////////////////////////////////////////////////////////////////////////////////////

void RenderPipelineCollection::Init() {
	CreateDefaultDesc();
	CreateDeffered();
	CreateForward();
	CreatePresent();
}

void RenderPipelineCollection::Term() {
}

void RenderPipelineCollection::SetPipeline(RenderPipelineType type, const DirectXThreadContext* context, const Vector2ui& size) {
	pipelines_[type]->ReloadAndSetPipeline(context, size);
}

void RenderPipelineCollection::BindGraphicsBuffer(RenderPipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) {
	pipelines_[type]->BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void RenderPipelineCollection::CreateDefaultDesc() {

	//* deffered 
	defferedDefaultDesc_ = {};
	defferedDefaultDesc_.CreateDefaultDesc();

	defferedDefaultDesc_.rtvFormats.clear();
	defferedDefaultDesc_.SetRTVFormats(static_cast<uint8_t>(SystematicRenderFrame::formats_.size()), SystematicRenderFrame::formats_.data());

	//* forward
	forwardDefaultDesc_ = {};
	forwardDefaultDesc_.CreateDefaultDesc();

}

void RenderPipelineCollection::CreateDeffered() {

	auto device = SxavengerSystem::GetDxDevice();

	{ //!< default.vs -> albedoDeferred.ps
		pipelines_[kDefaultVS_AlbedoPS_Deferred] = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipelines_[kDefaultVS_AlbedoPS_Deferred]->CreateAsset(kPackagesShaderDirectory / "sxavenger/behavior/default.vs.hlsl",        GraphicsShaderType::vs);
		pipelines_[kDefaultVS_AlbedoPS_Deferred]->CreateAsset(kPackagesShaderDirectory / "sxavenger/behavior/albedoDeferred.ps.hlsl", GraphicsShaderType::ps);
		pipelines_[kDefaultVS_AlbedoPS_Deferred]->RegisterBlob();

		pipelines_[kDefaultVS_AlbedoPS_Deferred]->ReflectionRootSignature(device);

		pipelines_[kDefaultVS_AlbedoPS_Deferred]->CreatePipeline(device, defferedDefaultDesc_);
	}

	{ //!< default.as -> default.ms -> albedo.ps
		pipelines_[kDefaultMS_AlbedoPS_Deferred] = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipelines_[kDefaultMS_AlbedoPS_Deferred]->CreateAsset(kPackagesShaderDirectory / "sxavenger/behavior/default.as.hlsl",        GraphicsShaderType::as);
		pipelines_[kDefaultMS_AlbedoPS_Deferred]->CreateAsset(kPackagesShaderDirectory / "sxavenger/behavior/default.ms.hlsl",        GraphicsShaderType::ms);
		pipelines_[kDefaultMS_AlbedoPS_Deferred]->CreateAsset(kPackagesShaderDirectory / "sxavenger/behavior/albedoDeferred.ps.hlsl", GraphicsShaderType::ps);
		pipelines_[kDefaultMS_AlbedoPS_Deferred]->RegisterBlob();

		pipelines_[kDefaultMS_AlbedoPS_Deferred]->ReflectionRootSignature(device);

		pipelines_[kDefaultMS_AlbedoPS_Deferred]->CreatePipeline(device, defferedDefaultDesc_);
	}

}

void RenderPipelineCollection::CreateForward() {

	auto device = SxavengerSystem::GetDxDevice();

	{ //!< default.vs -> albedo.ps
		pipelines_[kDefaultVS_AlbedoPS] = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipelines_[kDefaultVS_AlbedoPS]->CreateAsset(kPackagesShaderDirectory / "sxavenger/behavior/default.vs.hlsl", GraphicsShaderType::vs);
		pipelines_[kDefaultVS_AlbedoPS]->CreateAsset(kPackagesShaderDirectory / "sxavenger/behavior/albedo.ps.hlsl",  GraphicsShaderType::ps);
		pipelines_[kDefaultVS_AlbedoPS]->RegisterBlob();

		pipelines_[kDefaultVS_AlbedoPS]->ReflectionRootSignature(device);

		pipelines_[kDefaultVS_AlbedoPS]->CreatePipeline(device, forwardDefaultDesc_);
	}

	{
		pipelines_[kDefaultMS_AlbedoPS] = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipelines_[kDefaultMS_AlbedoPS]->CreateAsset(kPackagesShaderDirectory / "sxavenger/behavior/default.as.hlsl", GraphicsShaderType::as);
		pipelines_[kDefaultMS_AlbedoPS]->CreateAsset(kPackagesShaderDirectory / "sxavenger/behavior/default.ms.hlsl", GraphicsShaderType::ms);
		pipelines_[kDefaultMS_AlbedoPS]->CreateAsset(kPackagesShaderDirectory / "sxavenger/behavior/albedo.ps.hlsl",  GraphicsShaderType::ps);
		pipelines_[kDefaultMS_AlbedoPS]->RegisterBlob();

		pipelines_[kDefaultMS_AlbedoPS]->ReflectionRootSignature(device);

		pipelines_[kDefaultMS_AlbedoPS]->CreatePipeline(device, forwardDefaultDesc_);
	}
}

void RenderPipelineCollection::CreatePresent() {

	auto device = SxavengerSystem::GetDxDevice();

	pipelines_[kPresentToScreen] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[kPresentToScreen]->CreateAsset(kPackagesShaderDirectory / "sxavenger/present/present.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[kPresentToScreen]->CreateAsset(kPackagesShaderDirectory / "sxavenger/present/present.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[kPresentToScreen]->RegisterBlob();

	pipelines_[kPresentToScreen]->ReflectionRootSignature(device);

	DxObject::GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();

	desc.rtvFormats.clear();
	desc.SetRTVFormat(kScreenFormat);

	pipelines_[kPresentToScreen]->CreatePipeline(device, desc);

}
