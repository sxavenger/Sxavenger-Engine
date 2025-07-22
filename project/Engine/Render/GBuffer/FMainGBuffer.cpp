#include "FMainGBuffer.h"

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FMainGBuffer::kLayoutCount_> FMainGBuffer::kFormats_ = {
	FMainGBuffer::kColorFormat, //!< Scene
	FMainGBuffer::kColorFormat, //!< UI
};

////////////////////////////////////////////////////////////////////////////////////////////
// FMainGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FMainGBuffer::Init(const Vector2ui& size) {
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		buffers_[i] = std::make_unique<FBaseTexture>();
		buffers_[i]->Create(size, kFormats_[i]);
	}
}

void FMainGBuffer::TransitionBeginRenderTargetScene(const DirectXQueueContext* context, const D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilHandle) {

	auto commandList = context->GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Scene)]->TransitionBeginRenderTarget();

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 1> handles = {};
	handles[0] = buffers_[static_cast<size_t>(Layout::Scene)]->GetCPUHandleRTV();

	commandList->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(), false,
		&depthStencilHandle
	);
}

void FMainGBuffer::TransitionEndRenderTargetScene(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Scene)]->TransitionEndRenderTarget();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FMainGBuffer::ClearRenderTargetScene(const DirectXQueueContext* context) {
	buffers_[static_cast<size_t>(Layout::Scene)]->ClearRenderTarget(context);
}

void FMainGBuffer::TransitionBeginUnorderedScene(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Scene)]->TransitionBeginUnordered();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FMainGBuffer::TransitionEndUnorderedScene(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Scene)]->TransitionEndUnordered();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FMainGBuffer::TransitionBeginRenderTargetUI(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::UI)]->TransitionBeginRenderTarget();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FMainGBuffer::TransitionEndRenderTargetUI(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::UI)]->TransitionEndRenderTarget();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FMainGBuffer::ClearRenderTargetUI(const DirectXQueueContext* context) {
	buffers_[static_cast<size_t>(Layout::UI)]->ClearRenderTarget(context);
}

void FMainGBuffer::TransitionBeginUnorderedUI(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::UI)]->TransitionBeginUnordered();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FMainGBuffer::TransitionEndUnorderedUI(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::UI)]->TransitionEndUnordered();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

FBaseTexture* FMainGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[static_cast<size_t>(layout)].get();
}

DXGI_FORMAT FMainGBuffer::GetFormat(Layout layout) {
	return kFormats_[static_cast<size_t>(layout)];
}
