#include "FPresenter.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FPresenter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPresenter::Init() {
	pipeline_ = std::make_unique<DxObject::ReflectionGraphicsPipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/presenter/presenter.vs.hlsl", DxObject::GraphicsShaderType::vs);
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/presenter/presenter.ps.hlsl", DxObject::GraphicsShaderType::ps);
	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	DxObject::GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();

	desc.elements.clear();
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	desc.SetBlendMode(0, BlendMode::kBlendModeNormalSrc);

	desc.SetRTVFormat(0, DxObject::kScreenFormat);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

	vb_ = std::make_unique<DxObject::VertexDimensionBuffer<Vertex>>();
	vb_->Create(SxavengerSystem::GetDxDevice(), 3);
	vb_->At(0) = { { -1.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } };
	vb_->At(1) = { { 3.0f, 1.0f, 0.0f, 1.0f }, { 2.0f, 0.0f } };
	vb_->At(2) = { { -1.0f, -3.0f, 0.0f, 1.0f }, { 0.0f, 2.0f } };

}

void FPresenter::Present(const DirectXThreadContext* context, const Vector2ui& windowSize, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {

	auto command = context->GetDxCommand();

	pipeline_->SetPipeline(command, windowSize);

	D3D12_VERTEX_BUFFER_VIEW vbv = vb_->GetVertexBufferView();
	command->GetCommandList()->IASetVertexBuffers(0, 1, &vbv);

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gTexture", handle);

	pipeline_->BindGraphicsBuffer(command, desc);

	command->GetCommandList()->DrawInstanced(3, 1, 0, 0);


}
