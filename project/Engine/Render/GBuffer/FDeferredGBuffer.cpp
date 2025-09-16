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
	DXGI_FORMAT_R16G16B16A16_FLOAT  //!< Velocity
};

////////////////////////////////////////////////////////////////////////////////////////////
// FDeferredGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FDeferredGBuffer::Init(const Vector2ui& size) {
	for (size_t i = 0; i < buffers_.size(); ++i) {
		for (size_t j = 0; j < kLayoutCount_; ++j) {
			buffers_[i][j] = std::make_unique<FBaseTexture>();
			buffers_[i][j]->Create(size, kFormats_[j]);

			// nameの設定
			std::string name = "FDeferredGBuffer | ";
			name += magic_enum::enum_name(static_cast<FDeferredGBuffer::Layout>(j));
			buffers_[i][j]->GetResource()->SetName(ToWString(name).c_str());
		}
	}
}

void FDeferredGBuffer::TransitionBeginRenderTarget(const DirectXQueueContext* context, const D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilHandle) {

	auto commandList = context->GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount_> barriers = {};
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		barriers[i] = buffers_[currentBufferIndex_][i]->TransitionBeginRenderTarget();
	}

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kLayoutCount_> handles = {};
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		handles[i] = buffers_[currentBufferIndex_][i]->GetCPUHandleRTV();
	}

	commandList->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(), false,
		&depthStencilHandle
	);
}

void FDeferredGBuffer::TransitionEndRenderTarget(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount_> barriers = {};
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		barriers[i] = buffers_[currentBufferIndex_][i]->TransitionEndRenderTarget();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FDeferredGBuffer::ClearRenderTarget(const DirectXQueueContext* context) {
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		buffers_[currentBufferIndex_][i]->ClearRenderTarget(context);
	}
}

void FDeferredGBuffer::TransitionBeginUnordered(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount_> barriers = {};
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		barriers[i] = buffers_[currentBufferIndex_][i]->TransitionBeginUnordered();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FDeferredGBuffer::TransitionEndUnordered(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, kLayoutCount_> barriers = {};
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		barriers[i] = buffers_[currentBufferIndex_][i]->TransitionEndUnordered();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FDeferredGBuffer::SwapBuffers() {
	currentBufferIndex_ = (currentBufferIndex_ + 1) % buffers_.size();
}

FBaseTexture* FDeferredGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[currentBufferIndex_][static_cast<size_t>(layout)].get();
}

FBaseTexture* FDeferredGBuffer::GetPrevGBuffer(Layout layout) const {
	size_t prevIndex = (currentBufferIndex_ + buffers_.size() - 1) % buffers_.size();
	return buffers_[prevIndex][static_cast<size_t>(layout)].get();
}

DXGI_FORMAT FDeferredGBuffer::GetFormat(Layout layout) {
	return kFormats_[static_cast<size_t>(layout)];
}
