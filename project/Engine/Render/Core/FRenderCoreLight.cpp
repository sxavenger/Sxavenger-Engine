#include "FRenderCoreLight.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderTargetTextures.h"

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

	GraphicsPipelineDesc desc2d = {};
	desc2d.CreateDefaultDesc();
	desc2d.ClearElement();
	desc2d.SetDepthStencil(false);
	desc2d.SetRTVFormat(0, FRenderTargetTextures::GetFormat(FRenderTargetTextures::GBufferLayout::Main));
	desc2d.SetIndependentBlendEnable(false);
	desc2d.SetBlendDesc(0, blend);

	pipelines_[LightType::Empty] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Empty]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Empty]->CreateAsset(kDirectory_ / "Empty.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Empty]->RegisterBlob();
	pipelines_[LightType::Empty]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Empty]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	pipelines_[LightType::AlbedoEmpty] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::AlbedoEmpty]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::AlbedoEmpty]->CreateAsset(kDirectory_ / "Albedo.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::AlbedoEmpty]->RegisterBlob();
	pipelines_[LightType::AlbedoEmpty]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::AlbedoEmpty]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	pipelines_[LightType::Directional] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Directional]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Directional]->CreateAsset(kDirectory_ / "DirectionalLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Directional]->RegisterBlob();
	pipelines_[LightType::Directional]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Directional]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	pipelines_[LightType::Point] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Point]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Point]->CreateAsset(kDirectory_ / "PointLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Point]->RegisterBlob();
	pipelines_[LightType::Point]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Point]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	pipelines_[LightType::Spot] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Spot]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Spot]->CreateAsset(kDirectory_ / "SpotLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Spot]->RegisterBlob();
	pipelines_[LightType::Spot]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Spot]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	pipelines_[LightType::Rect] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Rect]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Rect]->CreateAsset(kDirectory_ / "RectLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Rect]->RegisterBlob();
	pipelines_[LightType::Rect]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Rect]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gBRDFSampler", SamplerMode::MODE_CLAMP);

	pipelines_[LightType::SkyLight] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::SkyLight]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::SkyLight]->CreateAsset(kDirectory_ / "skyLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::SkyLight]->RegisterBlob();
	pipelines_[LightType::SkyLight]->ReflectionRootSignature(SxavengerSystem::GetDxDevice(), desc);
	pipelines_[LightType::SkyLight]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);


}
