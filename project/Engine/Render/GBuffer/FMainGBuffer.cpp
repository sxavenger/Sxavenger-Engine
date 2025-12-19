#include "FMainGBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Convert.h>
#include <Engine/System/System.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FMainGBuffer::kLayoutCount> FMainGBuffer::kFormats = {
	FMainGBuffer::kColorFormat, //!< Scene
	FMainGBuffer::kColorFormat, //!< UI
};

////////////////////////////////////////////////////////////////////////////////////////////
// FMainGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FMainGBuffer::Init(const Vector2ui& size) {
	for (size_t i = 0; i < kLayoutCount; ++i) {
		buffers_[i] = std::make_unique<FBaseTexture>(size, kFormats[i], FBaseTexture::Flag::All);

		// nameの設定
		std::string name = "FMainGBuffer | ";
		name += magic_enum::enum_name(static_cast<FMainGBuffer::Layout>(i));
		buffers_[i]->GetResource()->SetName(ToWString(name).c_str());
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

void FMainGBuffer::TransitionBeginRenderTargetUI(const DirectXQueueContext* context, const D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilHandle) {

	auto commandList = context->GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Canvas)]->TransitionBeginRenderTarget();

	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 1> handles = {};
	handles[0] = buffers_[static_cast<size_t>(Layout::Canvas)]->GetCPUHandleRTV();

	commandList->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(), false,
		&depthStencilHandle
	);

}

void FMainGBuffer::TransitionEndRenderTargetUI(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Canvas)]->TransitionEndRenderTarget();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FMainGBuffer::ClearRenderTargetUI(const DirectXQueueContext* context) {
	buffers_[static_cast<size_t>(Layout::Canvas)]->ClearRenderTarget(context);
}

void FMainGBuffer::TransitionBeginUnorderedUI(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Canvas)]->TransitionBeginUnordered();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void FMainGBuffer::TransitionEndUnorderedUI(const DirectXQueueContext* context) {
	std::array<D3D12_RESOURCE_BARRIER, 1> barriers = {};
	barriers[0] = buffers_[static_cast<size_t>(Layout::Canvas)]->TransitionEndUnordered();

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

FBaseTexture* FMainGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[static_cast<size_t>(layout)].get();
}

DXGI_FORMAT FMainGBuffer::GetFormat(Layout layout) {
	return kFormats[static_cast<size_t>(layout)];
}
