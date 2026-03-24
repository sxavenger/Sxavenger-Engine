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

	CreateTransition(Transition::MotionVectorTransition, kDirectory / "MotionVectorTransition.cs.hlsl");

	CreateTransition(Transition::LightingTransition,    kDirectory / "LightingTransition.cs.hlsl");
	CreateTransition(Transition::ReflectionTransition,  kDirectory / "ReflectionTransition.cs.hlsl");
	CreateTransition(Transition::MipmapTransition,      kDirectory / "MipmapTransition.cs.hlsl");
	CreateTransition(Transition::TransparentTransition, kDirectory / "TransparentTransition.cs.hlsl");
}

void FRenderCoreTransition::SetPipeline(Transition transition, const DirectXQueueContext* context) const {
	transitions_[static_cast<size_t>(transition)].SetPipeline(context->GetDxCommand());
}

void FRenderCoreTransition::BindComputeBuffer(Transition transition, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	transitions_[static_cast<size_t>(transition)].BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreTransition::Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const {
	context->GetCommandList()->Dispatch(DxObject::RoundUp(resolution.x, kNumthread.x), DxObject::RoundUp(resolution.y, kNumthread.y), 1);
}

void FRenderCoreTransition::CreatePresent() {

	presenter_.CreateBlob(kDirectory / "presenter.vs.hlsl", DxObject::GraphicsShaderType::Vertex);
	presenter_.CreateBlob(kDirectory / "presenter.ps.hlsl", DxObject::GraphicsShaderType::Pixel);
	presenter_.ReflectionRootSignature(System::GetDxDevice(), D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED);

	DxObject::GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();

	desc.elements.clear();

	desc.SetDepthStencil(false);
	desc.SetRTVFormat(0, DxObject::kDefaultScreenViewFormat);

	presenter_.CreatePipeline(System::GetDxDevice(), desc);

}

void FRenderCoreTransition::CreateTransition(Transition transition, const std::filesystem::path& filepath) {
	auto& pipeline = transitions_[static_cast<size_t>(transition)];
	pipeline.CreateContent(filepath);
	pipeline.RegisterBlob();
	pipeline.ReflectionPipeline(System::GetDxDevice());
}

void FRenderCoreTransition::Present(const DirectXQueueContext* context, const Vector2ui& resolution, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) const {

	auto command = context->GetDxCommand();

	presenter_.SetPipeline(command, resolution);

	DxObject::BindBufferDesc desc = {};
	desc.SetHandle("gTexture", handle);

	presenter_.BindGraphicsBuffer(command, desc);
	command->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}
