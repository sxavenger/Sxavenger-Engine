#include "SpriteCommon.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerDirectory.h>
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteCommon class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SpriteCommon::Init() {
	pipeline_ = std::make_unique<ReflectionGraphicsPipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "sprite/sprite.vs.hlsl", GraphicsShaderType::vs);
	pipeline_->CreateBlob(kPackagesShaderDirectory / "sprite/sprite.ps.hlsl", GraphicsShaderType::ps);
	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();
	desc.ClearElement();
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	desc.SetElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

	buffer_ = std::make_unique<DimensionBuffer<Matrix4x4>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), 1);
	buffer_->At(0) = Matrix::MakeOrthographic(0.0f, 0.0f, static_cast<float>(kMainWindowSize.x), static_cast<float>(kMainWindowSize.y), 0.0f, 120.0f);
}

void SpriteCommon::SetPipeline(const DirectXThreadContext* context) const {
	pipeline_->SetPipeline(context->GetDxCommand());
}

void SpriteCommon::BindBuffer(const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) const {
	BindBufferDesc parameter = desc;
	parameter.SetAddress("gProjection", buffer_->GetGPUVirtualAddress());
	pipeline_->BindGraphicsBuffer(context->GetDxCommand(), parameter);
}
