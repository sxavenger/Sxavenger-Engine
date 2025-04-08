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
	CreateInputAssembler();
	CreatePipeline();
}

void FRenderCoreLight::BindIABuffer(const DirectXThreadContext* context) const {
	ia_.BindIABuffer(context);
}

void FRenderCoreLight::DrawCall(const DirectXThreadContext* context) const {
	ia_.DrawCall(context);
}

void FRenderCoreLight::SetPipeline(LightType type, const DirectXThreadContext* context, const Vector2ui& size) {
	pipelines_[type]->SetPipeline(context->GetDxCommand(), size);
}

void FRenderCoreLight::BindGraphicsBuffer(LightType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) {
	pipelines_[type]->BindGraphicsBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreLight::CreateInputAssembler() {
	ia_.Create(3, 1);
	ia_.GetVertex()->At(0) = { -1.0f, 1.0f };
	ia_.GetVertex()->At(1) = { 3.0f, 1.0f };
	ia_.GetVertex()->At(2) = { -1.0f, -3.0f };
	ia_.GetIndex()->At(0) = { 0, 2, 1 };
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
	desc2d.SetElement("POSITION", 0, DXGI_FORMAT_R32G32_FLOAT);
	desc2d.SetDepthStencil(false);
	desc2d.SetRTVFormat(0, FRenderTargetTextures::GetFormat(FRenderTargetTextures::GBufferLayout::Main));
	desc2d.SetIndependentBlendEnable(false);
	desc2d.SetBlendDesc(0, blend);

	pipelines_[LightType::Empty] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Empty]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Empty]->CreateAsset(kDirectory_ / "empty.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Empty]->RegisterBlob();
	pipelines_[LightType::Empty]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Empty]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	pipelines_[LightType::AlbedoEmpty] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::AlbedoEmpty]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::AlbedoEmpty]->CreateAsset(kDirectory_ / "albedo.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::AlbedoEmpty]->RegisterBlob();
	pipelines_[LightType::AlbedoEmpty]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::AlbedoEmpty]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	pipelines_[LightType::Directional] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Directional]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Directional]->CreateAsset(kDirectory_ / "directionalLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Directional]->RegisterBlob();
	pipelines_[LightType::Directional]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Directional]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	pipelines_[LightType::Point] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Point]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Point]->CreateAsset(kDirectory_ / "pointLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Point]->RegisterBlob();
	pipelines_[LightType::Point]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Point]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	pipelines_[LightType::Spot] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::Spot]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::Spot]->CreateAsset(kDirectory_ / "spotLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::Spot]->RegisterBlob();
	pipelines_[LightType::Spot]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::Spot]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);

	pipelines_[LightType::SkyLight] = std::make_unique<CustomReflectionGraphicsPipeline>();
	pipelines_[LightType::SkyLight]->CreateAsset(kDirectory_ / "lightRender2d.vs.hlsl", GraphicsShaderType::vs);
	pipelines_[LightType::SkyLight]->CreateAsset(kDirectory_ / "skyLight.ps.hlsl", GraphicsShaderType::ps);
	pipelines_[LightType::SkyLight]->RegisterBlob();
	pipelines_[LightType::SkyLight]->ReflectionRootSignature(SxavengerSystem::GetDxDevice());
	pipelines_[LightType::SkyLight]->CreatePipeline(SxavengerSystem::GetDxDevice(), desc2d);


}
