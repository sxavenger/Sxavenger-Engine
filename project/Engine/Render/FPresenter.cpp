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
	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice(), D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED);

	DxObject::GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();

	desc.elements.clear();

	desc.SetDepthStencil(false);
	desc.SetRTVFormat(0, DxObject::kDefaultScreenViewFormat);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
}

void FPresenter::Present(const DirectXQueueContext* context, const Vector2ui& windowSize, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {

	auto command = context->GetDxCommand();

	pipeline_->SetPipeline(command, windowSize);

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gTexture", handle);

	pipeline_->BindGraphicsBuffer(command, desc);

	command->GetCommandList()->DrawInstanced(3, 1, 0, 0);


}
