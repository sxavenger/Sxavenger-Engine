#include "RenderPipelineCollection.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Module/SxavengerGraphics/SystematicRenderFrame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPipelineCollection class
////////////////////////////////////////////////////////////////////////////////////////////

void RenderPipelineCollection::Init() {
	CreateDefaultDesc();
	CreateDeffered();
	CreateForward();
}

void RenderPipelineCollection::Term() {
}

void RenderPipelineCollection::SetPipeline(RenderPipelineType type, const DirectXThreadContext* context, const Vector2ui& size) {
	pipelines_[type]->ReloadAndSetPipeline(context->GetDxCommand(), size);
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
		pipelines_[kDefaultVS_AlbedoPS_Deferred] = std::make_unique<ReflectionGraphicsPipelineState>();
		pipelines_[kDefaultVS_AlbedoPS_Deferred]->CreateBlob("sxavenger/behavior/default.vs.hlsl",        GraphicsShaderType::vs);
		pipelines_[kDefaultVS_AlbedoPS_Deferred]->CreateBlob("sxavenger/behavior/albedoDeferred.ps.hlsl", GraphicsShaderType::ps);

		pipelines_[kDefaultVS_AlbedoPS_Deferred]->ReflectionRootSignature(device);

		pipelines_[kDefaultVS_AlbedoPS_Deferred]->CreatePipeline(device, defferedDefaultDesc_);
	}

	{ //!< default.as -> default.ms -> albedo.ps
		pipelines_[kDefaultMS_AlbedoPS_Deferred] = std::make_unique<ReflectionGraphicsPipelineState>();
		pipelines_[kDefaultMS_AlbedoPS_Deferred]->CreateBlob("sxavenger/behavior/default.as.hlsl",        GraphicsShaderType::as);
		pipelines_[kDefaultMS_AlbedoPS_Deferred]->CreateBlob("sxavenger/behavior/default.ms.hlsl",        GraphicsShaderType::ms);
		pipelines_[kDefaultMS_AlbedoPS_Deferred]->CreateBlob("sxavenger/behavior/albedoDeferred.ps.hlsl", GraphicsShaderType::ps);

		pipelines_[kDefaultMS_AlbedoPS_Deferred]->ReflectionRootSignature(device);

		pipelines_[kDefaultMS_AlbedoPS_Deferred]->CreatePipeline(device, defferedDefaultDesc_);
	}

}

void RenderPipelineCollection::CreateForward() {

	auto device = SxavengerSystem::GetDxDevice();

	{ //!< default.vs -> albedo.ps
		pipelines_[kDefaultVS_AlbedoPS] = std::make_unique<ReflectionGraphicsPipelineState>();
		pipelines_[kDefaultVS_AlbedoPS]->CreateBlob(L"sxavenger/behavior/default.vs.hlsl", GraphicsShaderType::vs);
		pipelines_[kDefaultVS_AlbedoPS]->CreateBlob(L"sxavenger/behavior/albedo.ps.hlsl",  GraphicsShaderType::ps);

		pipelines_[kDefaultVS_AlbedoPS]->ReflectionRootSignature(device);

		pipelines_[kDefaultVS_AlbedoPS]->CreatePipeline(device, forwardDefaultDesc_);
	}

	{
		pipelines_[kDefaultMS_AlbedoPS] = std::make_unique<ReflectionGraphicsPipelineState>();
		pipelines_[kDefaultMS_AlbedoPS]->CreateBlob(L"sxavenger/behavior/default.as.hlsl", GraphicsShaderType::as);
		pipelines_[kDefaultMS_AlbedoPS]->CreateBlob(L"sxavenger/behavior/default.ms.hlsl", GraphicsShaderType::ms);
		pipelines_[kDefaultMS_AlbedoPS]->CreateBlob(L"sxavenger/behavior/albedo.ps.hlsl", GraphicsShaderType::ps);

		pipelines_[kDefaultMS_AlbedoPS]->ReflectionRootSignature(device);

		pipelines_[kDefaultMS_AlbedoPS]->CreatePipeline(device, forwardDefaultDesc_);
	}
}