#include "FRenderTargetBuffer.h"
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderTargetBuffer::Create(const Vector2ui& size) {

	// 引数の保存
	size_ = size;

	deferred_.Init(size_);
	lighting_.Init(size_);
	main_.Init(size_);

	depth_ = std::make_unique<FDepthTexture>();
	depth_->Create(size_);

	priority_ = std::make_unique<FPriorityTexture>();
	priority_->Create(size_);

	process_ = std::make_unique<FProcessTextureCollection>();
	process_->Create(3, size_, FMainGBuffer::kColorFormat);

	index_ = std::make_unique<DimensionBuffer<DeferredBufferIndex>>();
	index_->Create(SxavengerSystem::GetDxDevice(), 1);
	AttachIndex();
}

void FRenderTargetBuffer::BeginRenderTargetDeferred(const DirectXQueueContext* context) {
	// OMSetRenderTargetsの設定
	depth_->TransitionBeginRasterizer(context);
	deferred_.TransitionBeginRenderTarget(context, depth_->GetRasterizerCPUHandleDSV());

	// ClearRenderTargetの設定
	depth_->ClearRasterizerDepth(context);
	deferred_.ClearRenderTarget(context);
}

void FRenderTargetBuffer::EndRenderTargetDeferred(const DirectXQueueContext* context) {
	depth_->TransitionEndRasterizer(context);
	deferred_.TransitionEndRenderTarget(context);
}

void FRenderTargetBuffer::BeginUnorderedMainScene(const DirectXQueueContext* context) {
	main_.TransitionBeginUnorderedScene(context);
}

void FRenderTargetBuffer::EndUnorderedMainScene(const DirectXQueueContext* context) {
	main_.TransitionEndUnorderedScene(context);
}

void FRenderTargetBuffer::BeginRenderTargetMainTransparent(const DirectXQueueContext* context) {
	depth_->TransitionBeginRasterizer(context);
	main_.TransitionBeginRenderTargetScene(context, depth_->GetRasterizerCPUHandleDSV());
}

void FRenderTargetBuffer::EndRenderTargetMainTransparent(const DirectXQueueContext* context) {
	depth_->TransitionEndRasterizer(context);
	main_.TransitionEndRenderTargetScene(context);
}

void FRenderTargetBuffer::BeginRenderTargetMainUI(const DirectXQueueContext* context) {
	main_.TransitionBeginRenderTargetUI(context, priority_->GetCPUHandleDSV());
	main_.ClearRenderTargetUI(context);
	priority_->ClearDepth(context);
}

void FRenderTargetBuffer::EndRenderTargetMainUI(const DirectXQueueContext* context) {
	main_.TransitionEndRenderTargetUI(context);
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

FBaseTexture* FRenderTargetBuffer::GetGBuffer(FMainGBuffer::Layout layout) {
	return main_.GetGBuffer(layout);
}

const D3D12_GPU_VIRTUAL_ADDRESS& FRenderTargetBuffer::GetIndexBufferAddress() const {
	return index_->GetGPUVirtualAddress();
}

void FRenderTargetBuffer::AttachIndex() {
	auto& parameter = index_->At(0);
	parameter.albedo      = GetGBuffer(FDeferredGBuffer::Layout::Albedo)->GetDescriptorSRV().GetIndex();
	parameter.normal      = GetGBuffer(FDeferredGBuffer::Layout::Normal)->GetDescriptorSRV().GetIndex();
	parameter.materialARM = GetGBuffer(FDeferredGBuffer::Layout::MaterialARM)->GetDescriptorSRV().GetIndex();
	parameter.position    = GetGBuffer(FDeferredGBuffer::Layout::Position)->GetDescriptorSRV().GetIndex();
	parameter.depth       = depth_->GetRasterizerDescriptorSRV().GetIndex();
}
