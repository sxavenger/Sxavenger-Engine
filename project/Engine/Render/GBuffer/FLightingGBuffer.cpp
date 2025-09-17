#include "FLightingGBuffer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* GBuffer
#include "FMainGBuffer.h"

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FLightingGBuffer::kLayoutCount_> FLightingGBuffer::kFormats_ = {
	FMainGBuffer::kColorFormat, //!< Direct
	FMainGBuffer::kColorFormat, //!< Indirect_Reservoir
	DXGI_FORMAT_R32_UINT,       //!< Indirect_Moment
	FMainGBuffer::kColorFormat, //!< Indirect
};

////////////////////////////////////////////////////////////////////////////////////////////
// FLightingGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FLightingGBuffer::Init(const Vector2ui& size) {
	for (size_t i = 0; i < kLayoutCount_; ++i) {
		buffers_[i] = std::make_unique<FBaseTexture>();
		buffers_[i]->Create(size, kFormats_[i]);

		// nameの設定
		std::string name = "FLightingGBuffer | ";
		name += magic_enum::enum_name(static_cast<FLightingGBuffer::Layout>(i));
		buffers_[i]->GetResource()->SetName(ToWString(name).c_str());
	}
}

void FLightingGBuffer::TransitionBeginRenderTargetDirect(const DirectXQueueContext* context) {

	auto commandList = context->GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Direct)]->TransitionBeginRenderTarget();

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 1> handles = {};
	handles[0] = buffers_[static_cast<size_t>(Layout::Direct)]->GetCPUHandleRTV();

	commandList->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(), false,
		nullptr
	);
}

void FLightingGBuffer::TransitionEndRenderTargetDirect(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Direct)]->TransitionEndRenderTarget();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FLightingGBuffer::ClearRenderTargetDirect(const DirectXQueueContext* context) {
	buffers_[static_cast<size_t>(Layout::Direct)]->ClearRenderTarget(context);
}

void FLightingGBuffer::TransitionBeginUnorderedDirect(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Direct)]->TransitionBeginUnordered();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FLightingGBuffer::TransitionEndUnorderedDirect(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Direct)]->TransitionEndUnordered();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FLightingGBuffer::TransitionBeginRenderTargetIndirect(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Indirect)]->TransitionBeginRenderTarget();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FLightingGBuffer::TransitionEndRenderTargetIndirect(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Indirect)]->TransitionEndRenderTarget();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FLightingGBuffer::ClearRenderTargetIndirect(const DirectXQueueContext* context) {
	buffers_[static_cast<size_t>(Layout::Indirect)]->ClearRenderTarget(context);
}

void FLightingGBuffer::TransitionBeginUnorderedIndirect(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Indirect_Reservoir)]->TransitionBeginUnordered();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FLightingGBuffer::TransitionEndUnorderedIndirect(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Indirect_Reservoir)]->TransitionEndUnordered();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

FBaseTexture* FLightingGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[static_cast<size_t>(layout)].get();
}

DXGI_FORMAT FLightingGBuffer::GetFormat(Layout layout) {
	return kFormats_[static_cast<size_t>(layout)];
}
