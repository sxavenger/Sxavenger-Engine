#include "FRenderTargetBuffer.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderTargetBuffer::Create(const Vector2ui& size) {

	// 引数の保存
	size_ = size;

	deferred_.Init(size_);
	lighting_.Init(size_);
	transparent_.Init(size_);
	main_.Init(size_);

	probe_.Init();

	depth_ = std::make_unique<FDepthTexture>();
	depth_->Create(size_);

	priority_ = std::make_unique<FPriorityTexture>();
	priority_->Create(size_);

	process_ = std::make_unique<FProcessTextureCollection>();
	process_->Create(3, size_, FMainGBuffer::kColorFormat);

	index_ = std::make_unique<ConstantBuffer<DeferredBufferIndex>>();
	index_->Create(System::GetDxDevice());
	AttachIndex();
}

void FRenderTargetBuffer::TransitionBeginRenderTargetMainScene(const DirectXQueueContext* context) {
	depth_->TransitionBeginRasterizer(context);
	main_.TransitionBeginRenderTarget(context, FMainGBuffer::Layout::Scene, depth_->GetRasterizerCPUHandleDSV());
}

void FRenderTargetBuffer::TransitionEndRenderTargetMainScene(const DirectXQueueContext* context) {
	depth_->TransitionEndRasterizer(context);
	main_.TransitionEndRenderTarget(context, FMainGBuffer::Layout::Scene);
}

void FRenderTargetBuffer::TransitionBeginUnorderedMainScene(const DirectXQueueContext* context) {
	main_.TransitionBeginUnordered(context, FMainGBuffer::Layout::Scene);
}

void FRenderTargetBuffer::TransitionEndUnorderedMainScene(const DirectXQueueContext* context) {
	main_.TransitionEndUnordered(context, FMainGBuffer::Layout::Scene);
}

void FRenderTargetBuffer::TransitionBeginRenderTargetMainCanvas(const DirectXQueueContext* context) {
	main_.TransitionBeginRenderTarget(context, FMainGBuffer::Layout::Canvas, priority_->GetCPUHandleDSV());
}

void FRenderTargetBuffer::TransitionEndRenderTargetMainCanvas(const DirectXQueueContext* context) {
	main_.TransitionEndRenderTarget(context, FMainGBuffer::Layout::Canvas);
}

void FRenderTargetBuffer::ClearRenderTargetMain(const DirectXQueueContext* context) {

	auto commandList = context->GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, 2> barriers = {
		main_.GetGBuffer(FMainGBuffer::Layout::Scene)->TransitionBeginRenderTarget(),
		main_.GetGBuffer(FMainGBuffer::Layout::Canvas)->TransitionBeginRenderTarget(),
	};

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	main_.ClearRenderTarget(context, FMainGBuffer::Layout::Scene);
	main_.ClearRenderTarget(context, FMainGBuffer::Layout::Canvas);

	barriers = {
		main_.GetGBuffer(FMainGBuffer::Layout::Scene)->TransitionEndRenderTarget(),
		main_.GetGBuffer(FMainGBuffer::Layout::Canvas)->TransitionEndRenderTarget(),
	};

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	depth_->TransitionBeginRasterizer(context);
	depth_->ClearRasterizerDepth(context);
	depth_->TransitionEndRasterizer(context);

	priority_->ClearDepth(context);
}

void FRenderTargetBuffer::BeginPostProcess(const DirectXQueueContext* context) {
	process_->BeginProcess(context, main_.GetGBuffer(FMainGBuffer::Layout::Scene));
}

void FRenderTargetBuffer::EndPostProcess(const DirectXQueueContext* context) {
	process_->EndProcess(context, main_.GetGBuffer(FMainGBuffer::Layout::Scene));
}

FBaseTexture* FRenderTargetBuffer::GetGBuffer(FDeferredGBuffer::Layout layout) {
	return deferred_.GetGBuffer(layout);
}

FBaseTexture* FRenderTargetBuffer::GetGBuffer(FLightingGBuffer::Layout layout) {
	return lighting_.GetGBuffer(layout);
}

FBaseTexture* FRenderTargetBuffer::GetGBuffer(FTransparentGBuffer::Layout layout) {
	return transparent_.GetGBuffer(layout);
}

FBaseTexture* FRenderTargetBuffer::GetGBuffer(FMainGBuffer::Layout layout) {
	return main_.GetGBuffer(layout);
}

const D3D12_GPU_VIRTUAL_ADDRESS& FRenderTargetBuffer::GetIndexBufferAddress() const {
	return index_->GetGPUVirtualAddress();
}

void FRenderTargetBuffer::AttachIndex() {
	auto& parameter = index_->At();
	parameter.albedo      = GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetDescriptorSRV().GetIndex();
	parameter.normal      = GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetDescriptorSRV().GetIndex();
	parameter.materialARM = GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetDescriptorSRV().GetIndex();
	parameter.position    = GetGBuffer(FDeferredGBuffer::Layout::Position)->GetDescriptorSRV().GetIndex();
	parameter.depth       = depth_->GetRasterizerDescriptorSRV().GetIndex();
}
