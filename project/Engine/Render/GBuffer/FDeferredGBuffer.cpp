#include "FDeferredGBuffer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* GBuffer
#include "FMainGBuffer.h"

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FDeferredGBuffer::kLayoutCount_> FDeferredGBuffer::kFormats_ = {
	FMainGBuffer::kColorFormat,     //!< Albedo
	DXGI_FORMAT_R10G10B10A2_UNORM,  //!< Normal
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< MaterialARM
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Position
};

////////////////////////////////////////////////////////////////////////////////////////////
// FDeferredGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FDeferredGBuffer::Init(const Vector2ui& size) {
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		buffers_[i] = std::make_unique<FBaseTexture>();
		buffers_[i]->Create(size, kFormats_[i]);

		// nameの設定
		std::string name = "FDeferredGBuffer | ";
		name += magic_enum::enum_name(static_cast<FDeferredGBuffer::Layout>(i));
		buffers_[i]->GetResource()->SetName(ToWString(name).c_str());
	}
}

void FDeferredGBuffer::TransitionBeginRenderTarget(const DirectXQueueContext* context, const D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilHandle) {

	auto commandList = context->GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount_> barriers = {};
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		barriers[i] = buffers_[i]->TransitionBeginRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kLayoutCount_> handles = {};
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		handles[i] = buffers_[i]->GetCPUHandleRTV();
	}

	commandList->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(), false,
		&depthStencilHandle
	);
}

void FDeferredGBuffer::TransitionEndRenderTarget(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount_> barriers = {};
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		barriers[i] = buffers_[i]->TransitionEndRenderTarget();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FDeferredGBuffer::ClearRenderTarget(const DirectXQueueContext* context) {
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		buffers_[i]->ClearRenderTarget(context);
	}
}

void FDeferredGBuffer::TransitionBeginUnordered(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount_> barriers = {};
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		barriers[i] = buffers_[i]->TransitionBeginUnordered();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FDeferredGBuffer::TransitionEndUnordered(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount_> barriers = {};
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		barriers[i] = buffers_[i]->TransitionEndUnordered();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

FBaseTexture* FDeferredGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[static_cast<size_t>(layout)].get();
}

DXGI_FORMAT FDeferredGBuffer::GetFormat(Layout layout) {
	return kFormats_[static_cast<size_t>(layout)];
}
