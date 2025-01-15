#include "APointLight.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Config/SxavengerDirectory.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ALightActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void APointLight::Init() {

	TransformComponent::CreateBuffer();

	cb_ = std::make_unique<DxObject::DimensionBuffer<Parameter>>();
	cb_->Create(SxavengerSystem::GetDxDevice(), 1);
	cb_->At(0).Init();

	TestInit();
}

void APointLight::Render(const RendererContext& context) {
	TestRender(context);
}

void APointLight::TestInit() {

	TransformComponent::GetTransform().scale = { 20.0f, 20.0f, 20.0f };
	TransformComponent::GetTransform().translate = { 0.0f, 2.0f, 0.0f };
	TransformComponent::UpdateMatrix();

	ia_.Create(8, 12);

	ia_.GetVertex()->At(0) = { -0.5f, -0.5f,  0.5f };
	ia_.GetVertex()->At(1) = { 0.5f, -0.5f,  0.5f };
	ia_.GetVertex()->At(2) = { 0.5f,  0.5f,  0.5f };
	ia_.GetVertex()->At(3) = { -0.5f,  0.5f,  0.5f };
	ia_.GetVertex()->At(4) = { -0.5f, -0.5f, -0.5f };
	ia_.GetVertex()->At(5) = { 0.5f, -0.5f, -0.5f };
	ia_.GetVertex()->At(6) = { 0.5f,  0.5f, -0.5f };
	ia_.GetVertex()->At(7) = { -0.5f,  0.5f, -0.5f };

	ia_.GetIndex()->At(0) = { 0, 1, 2 };
	ia_.GetIndex()->At(1) = { 0, 2, 3 };
	ia_.GetIndex()->At(2) = { 1, 5, 6 };
	ia_.GetIndex()->At(3) = { 1, 6, 2 };
	ia_.GetIndex()->At(4) = { 5, 4, 7 };
	ia_.GetIndex()->At(5) = { 5, 7, 6 };
	ia_.GetIndex()->At(6) = { 4, 0, 3 };
	ia_.GetIndex()->At(7) = { 4, 3, 7 };
	ia_.GetIndex()->At(8) = { 3, 2, 6 };
	ia_.GetIndex()->At(9) = { 3, 6, 7 };
	ia_.GetIndex()->At(10) = { 4, 5, 1 };
	ia_.GetIndex()->At(11) = { 4, 1, 0 };

	pipeline_ = std::make_unique<DxObject::ReflectionGraphicsPipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/light/baseLightRender.vs.hlsl", DxObject::GraphicsShaderType::vs);
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/light/pointLight.ps.hlsl", DxObject::GraphicsShaderType::ps);
	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	DxObject::GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();
	desc.elements.clear();
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	desc.SetDepthStencil(false);
	desc.SetRTVFormat(0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

}

void APointLight::TestRender(const RendererContext& context) {
	pipeline_->SetPipeline(context.context->GetDxCommand(), context.size);

	DxObject::BindBufferDesc parameter = context.parameter;
	parameter.SetAddress("gTransform",  TransformComponent::GetGPUVirtualAddress());
	parameter.SetAddress("gPointLight", cb_->GetGPUVirtualAddress());

	ia_.BindIABuffer(context.context);

	pipeline_->BindGraphicsBuffer(context.context->GetDxCommand(), parameter);

	ia_.DrawCall(context.context);

	
}
