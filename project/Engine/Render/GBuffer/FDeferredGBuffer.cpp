#include "FDeferredGBuffer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* GBuffer
#include "FMainGBuffer.h"

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FDeferredGBuffer::kLayoutCount> FDeferredGBuffer::kFormats_ = {
	FMainGBuffer::kColorFormat,     //!< Albedo
	DXGI_FORMAT_R10G10B10A2_UNORM,  //!< Normal
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< MaterialARM
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< Position
	DXGI_FORMAT_R16G16B16A16_FLOAT  //!< Velocity
};

////////////////////////////////////////////////////////////////////////////////////////////
// FDeferredGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FDeferredGBuffer::Init(const Vector2ui& size) {
	for (size_t j = 0; j < kLayoutCount; ++j) {
		buffers_[j] = std::make_unique<FBaseTexture>(size, kFormats_[j], FBaseTexture::Flag::All);

		// nameの設定
		std::string name = "FDeferredGBuffer | ";
		name += magic_enum::enum_name(static_cast<FDeferredGBuffer::Layout>(j));
		buffers_[j]->GetResource()->SetName(ToWString(name).c_str());
	}
}

void FDeferredGBuffer::TransitionBeginRenderTarget(const DirectXQueueContext* context, const D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilHandle) {

	auto commandList = context->GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount> barriers = {};
	for (size_t i = 0; i < kLayoutCount; ++i) {
		barriers[i] = buffers_[i]->TransitionBeginRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kLayoutCount> handles = {};
	for (size_t i = 0; i < kLayoutCount; ++i) {
		handles[i] = buffers_[i]->GetCPUHandleRTV();
	}

	commandList->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(), false,
		&depthStencilHandle
	);
}

void FDeferredGBuffer::TransitionEndRenderTarget(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount> barriers = {};
	for (size_t i = 0; i < kLayoutCount; ++i) {
		barriers[i] = buffers_[i]->TransitionEndRenderTarget();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FDeferredGBuffer::ClearRenderTarget(const DirectXQueueContext* context) {
	for (size_t i = 0; i < kLayoutCount; ++i) {
		buffers_[i]->ClearRenderTarget(context);
	}
}

void FDeferredGBuffer::TransitionBeginUnordered(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount> barriers = {};
	for (size_t i = 0; i < kLayoutCount; ++i) {
		barriers[i] = buffers_[i]->TransitionBeginUnordered();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FDeferredGBuffer::TransitionEndUnordered(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount> barriers = {};
	for (size_t i = 0; i < kLayoutCount; ++i) {
		barriers[i] = buffers_[i]->TransitionEndUnordered();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FDeferredGBuffer::ForEach(const std::function<void(FBaseTexture*)>& function) {
	for (size_t i = 0; i < kLayoutCount; ++i) {
		function(buffers_[i].get());
	}
}

void FDeferredGBuffer::ForEach(const std::function<void(size_t, FBaseTexture*)>& function) {
	for (size_t i = 0; i < kLayoutCount; ++i) {
		function(i, buffers_[i].get());
	}
}

FBaseTexture* FDeferredGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[static_cast<size_t>(layout)].get();
}

DXGI_FORMAT FDeferredGBuffer::GetFormat(Layout layout) {
	return kFormats_[static_cast<size_t>(layout)];
}
