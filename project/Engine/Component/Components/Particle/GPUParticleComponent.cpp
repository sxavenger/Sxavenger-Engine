#include "GPUParticleComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GPUParticleComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GPUParticleComponent::Create(uint32_t count) {
	buffer_ = std::make_unique<DxObject::UnorderedDimensionBuffer<Element>>();
	buffer_->Create(SxavengerSystem::GetDxDevice(), count);

	freeList_ = std::make_unique<DxObject::AppendConsumeBuffer<uint32_t>>();
	freeList_->Create(SxavengerSystem::GetDxDevice(), SxavengerSystem::GetDxDescriptorHeaps(), count, 2);

	config_.Create(1);

	pipeline_ = std::make_unique<DxObject::ReflectionComputePipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/Particle/GPUParticle/update.cs.hlsl");
	pipeline_->ReflectionPipeline(SxavengerSystem::GetDxDevice());
}

void GPUParticleComponent::Update(const DirectXQueueContext* context) {
	pipeline_->SetPipeline(context->GetDxCommand());

	DxObject::BindBufferDesc desc = {};
	//* particle
	desc.SetAddress("gParticles", GetGPUVirtualAddress());
	desc.Set32bitConstants("Dimension", 2, &buffer_->GetDimension());

	//* free list
	desc.SetHandle("gAppendFreeIndex", freeList_->GetAppendCousumeGPUHandleUAV());

	//* time
	float time = SxavengerSystem::GetDeltaTime().time;
	desc.Set32bitConstants("Time", 1, &time);

	pipeline_->BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline_->Dispatch(context->GetDxCommand(), { DxObject::RoundUp(buffer_->GetSize(), 16), 1, 1 });

	buffer_->Barrier(context->GetDxCommand());
}

const D3D12_GPU_VIRTUAL_ADDRESS& GPUParticleComponent::GetGPUVirtualAddress() const {
	Exception::Assert(buffer_ != nullptr, "particle buffer is not created.");
	return buffer_->GetGPUVirtualAddress();
}

void GPUParticleComponent::SetPrimitive(InputPrimitive&& primitive) {
	primitive_ = std::move(primitive);
}

void GPUParticleComponent::BindIABuffer(const DirectXQueueContext* context) const {
	Exception::Assert(primitive_.has_value(), "particle primitive is not set.");
	(*primitive_).BindIABuffer(context);
}

void GPUParticleComponent::DrawCall(const DirectXQueueContext* context) const {
	Exception::Assert(primitive_.has_value(), "particle primitive is not set.");
	(*primitive_).DrawCall(context, buffer_->GetSize());
}

bool GPUParticleComponent::HasPritimive() const {
	return primitive_.has_value();
}
