#include "FRenderCoreTransition.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreTransition class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreTransition::Init() {
	CreatePresent();

	CreateTransition(Transition::LightingTransition, kDirectory / "LightingTransition.cs.hlsl");
	CreateTransition(Transition::MipmapTransition,   kDirectory / "MipmapTransition.cs.hlsl");
	CreateTransition(Transition::VelocityTransition, kDirectory / "VelocityTransition.cs.hlsl");
}

void FRenderCoreTransition::SetPipeline(Transition transition, const DirectXQueueContext* context) {
	transitions_[static_cast<size_t>(transition)]->SetPipeline(context->GetDxCommand());
}

void FRenderCoreTransition::BindComputeBuffer(Transition transition, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) {
	transitions_[static_cast<size_t>(transition)]->BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreTransition::Dispatch(const DirectXQueueContext* context, const Vector2ui& size) {
	context->GetCommandList()->Dispatch(DxObject::RoundUp(size.x, kNumThreadSize.x), DxObject::RoundUp(size.y, kNumThreadSize.y), 1);
}

void FRenderCoreTransition::CreatePresent() {

	presenter_ = std::make_unique<DxObject::ReflectionGraphicsPipelineState>();
	presenter_->CreateBlob(kPackagesDirectory / "shaders/render/presenter/presenter.vs.hlsl", DxObject::GraphicsShaderType::vs);
	presenter_->CreateBlob(kPackagesDirectory / "shaders/render/presenter/presenter.ps.hlsl", DxObject::GraphicsShaderType::ps);
	presenter_->ReflectionRootSignature(System::GetDxDevice(), D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED);

	DxObject::GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();

	desc.elements.clear();

	desc.SetDepthStencil(false);
	desc.SetRTVFormat(0, DxObject::kDefaultScreenViewFormat);

	presenter_->CreatePipeline(System::GetDxDevice(), desc);

}

void FRenderCoreTransition::CreateTransition(Transition transition, const std::filesystem::path& filepath) {
	auto& pipeline = transitions_[static_cast<size_t>(transition)];
	pipeline = std::make_unique<CustomReflectionComputePipeline>();
	pipeline->CreateContent(filepath);
	pipeline->RegisterBlob();
	pipeline->ReflectionPipeline(System::GetDxDevice());
}

void FRenderCoreTransition::Present(const DirectXQueueContext* context, const Vector2ui& windowSize, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {

	auto command = context->GetDxCommand();

	presenter_->SetPipeline(command, windowSize);

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gTexture", handle);

	presenter_->BindGraphicsBuffer(command, desc);
	command->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}
