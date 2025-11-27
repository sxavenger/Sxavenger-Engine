#include "FRenderCoreLight.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../GBuffer/FLightingGBuffer.h"

//* engine
#include <Engine/System/Config/SxavengerConfig.h>

//=========================================================================================
// public methods
//=========================================================================================

const std::filesystem::path FRenderCoreLight::kDirectory_ = kPackagesShaderDirectory / "render/light";

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreLight class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreLight::Init() {
	CreatePipeline();
}

void FRenderCoreLight::DrawCall(const DirectXQueueContext* context, UINT instanceCount) const {
	context->GetCommandList()->DrawInstanced(3, instanceCount, 0, 0);
}

void FRenderCoreLight::SetPipeline(LightType type, const DirectXQueueContext* context, const Vector2ui& size) {
	//pipelines_[type]->SetPipeline(context->GetDxCommand(), size);
	pipelines_[type]->ReloadAndSetPipeline(context, size);
}

void FRenderCoreLight::BindGraphicsBuffer(LightType type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) {
	pipelines_[type]->BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreLight::CreatePipeline() {

	D3D12_RENDER_TARGET_BLEND_DESC blend = {};
	blend.BlendEnable           = true;
	blend.LogicOpEnable         = false;
	blend.SrcBlend              = D3D12_BLEND_ONE;
	blend.DestBlend             = D3D12_BLEND_ONE;
	blend.BlendOp               = D3D12_BLEND_OP_ADD;
	blend.SrcBlendAlpha         = D3D12_BLEND_ONE;
	blend.DestBlendAlpha        = D3D12_BLEND_ZERO;
	blend.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
	blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	GraphicsPipelineDesc desc_front = {};
	desc_front.CreateDefaultDesc();
	desc_front.ClearElement();
	desc_front.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_GREATER); //!< Depthが1.0以外に書き込み
	desc_front.SetRTVFormat(0, FLightingGBuffer::GetFormat(FLightingGBuffer::Layout::Direct));
	desc_front.SetIndependentBlendEnable(false);
	desc_front.SetBlendDesc(0, blend);

	GraphicsPipelineDesc desc_back = {};
	desc_back.CreateDefaultDesc();
	desc_back.ClearElement();
	desc_back.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_EQUAL); //!< Depthが1.0に書き込み
	desc_back.SetRTVFormat(0, FLightingGBuffer::GetFormat(FLightingGBuffer::Layout::Direct));
	desc_back.SetIndependentBlendEnable(false);
	desc_back.SetBlendDesc(0, blend);

	pipelines_[LightType::Empty] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Empty]->CreateContent(kDirectory_ / "LightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Empty]->CreateContent(kDirectory_ / "Empty.ps.hlsl",         GraphicsShaderType::ps);
	pipelines_[LightType::Empty]->RegisterBlob();
	pipelines_[LightType::Empty]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Empty]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc_front);

	pipelines_[LightType::Directional] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Directional]->CreateContent(kDirectory_ / "LightRender2d.vs.hlsl",    GraphicsShaderType::vs);
	pipelines_[LightType::Directional]->CreateContent(kDirectory_ / "DirectionalLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Directional]->RegisterBlob();
	pipelines_[LightType::Directional]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Directional]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc_front);

	pipelines_[LightType::Point] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Point]->CreateContent(kDirectory_ / "LightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Point]->CreateContent(kDirectory_ / "PointLight.ps.hlsl",    GraphicsShaderType::ps);
	pipelines_[LightType::Point]->RegisterBlob();
	pipelines_[LightType::Point]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Point]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc_front);

	pipelines_[LightType::Spot] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Spot]->CreateContent(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Spot]->CreateContent(kDirectory_ / "SpotLight.ps.hlsl",     GraphicsShaderType::ps);
	pipelines_[LightType::Spot]->RegisterBlob();
	pipelines_[LightType::Spot]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Spot]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc_front);

	{
		SamplerBindDesc desc = {};
		desc.SetSamplerLinear("gBRDFSampler", SamplerMode::MODE_CLAMP);

		pipelines_[LightType::SkyLight] = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipelines_[LightType::SkyLight]->CreateContent(kDirectory_ / "LightRender2d.vs.hlsl", GraphicsShaderType::vs);
		pipelines_[LightType::SkyLight]->CreateContent(kDirectory_ / "SkyLight.ps.hlsl",      GraphicsShaderType::ps);
		pipelines_[LightType::SkyLight]->RegisterBlob();
		pipelines_[LightType::SkyLight]->ReflectionRootSignature(SxavengerSystem::GetDxDevice(), desc);
		pipelines_[LightType::SkyLight]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc_front);

		pipelines_[LightType::VisibilitySkyLight] = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipelines_[LightType::VisibilitySkyLight]->CreateContent(kDirectory_ / "LightRender2d.vs.hlsl",     GraphicsShaderType::vs);
		pipelines_[LightType::VisibilitySkyLight]->CreateContent(kDirectory_ / "VisibilitySkyLight.ps.hlsl", GraphicsShaderType::ps);
		pipelines_[LightType::VisibilitySkyLight]->RegisterBlob();
		pipelines_[LightType::VisibilitySkyLight]->ReflectionRootSignature(SxavengerSystem::GetDxDevice(), desc);
		pipelines_[LightType::VisibilitySkyLight]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc_back);


		pipelines_[LightType::SkyLightEnvironment] = std::make_unique<CustomReflectionGraphicsPipeline>();
		pipelines_[LightType::SkyLightEnvironment]->CreateContent(kDirectory_ / "LightRender2d.vs.hlsl",       GraphicsShaderType::vs);
		pipelines_[LightType::SkyLightEnvironment]->CreateContent(kDirectory_ / "SkyLightEnvironment.ps.hlsl", GraphicsShaderType::ps);
		pipelines_[LightType::SkyLightEnvironment]->RegisterBlob();
		pipelines_[LightType::SkyLightEnvironment]->ReflectionRootSignature(SxavengerSystem::GetDxDevice(), desc);
		pipelines_[LightType::SkyLightEnvironment]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc_back);



	}
	
}
