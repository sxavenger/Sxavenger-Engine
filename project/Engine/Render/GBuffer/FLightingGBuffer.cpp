#include "FLightingGBuffer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* GBuffer
#include "FMainGBuffer.h"

//* render
#include "../Core/FRenderCorePathtracing.h"

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FLightingGBuffer::kLayoutCount_> FLightingGBuffer::kFormats_ = {
	FMainGBuffer::kColorFormat,    //!< Direct
	FMainGBuffer::kColorFormat,    //!< Indirect_Atlas_Diffuse
	FMainGBuffer::kColorFormat,    //!< Indirect_Atlas_Specular
	FMainGBuffer::kColorFormat,    //!< Indirect_Reservoir_Diffuse
	FMainGBuffer::kColorFormat,    //!< Indirect_Reservoir_Specular
	DXGI_FORMAT_R32G32B32A32_UINT, //!< Indirect_Moment
	FMainGBuffer::kColorFormat,    //!< Indirect
};

////////////////////////////////////////////////////////////////////////////////////////////
// FLightingGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FLightingGBuffer::Init(const Vector2ui& size) {

	//!< 最終Lighting結果格納用
	buffers_[static_cast<uint8_t>(Layout::Direct)]   = std::make_unique<FBaseTexture>(size, GetFormat(Layout::Direct));
	buffers_[static_cast<uint8_t>(Layout::Indirect)] = std::make_unique<FBaseTexture>(size, GetFormat(Layout::Indirect));

	downsize_ = size / 4u;

	//!< Indirect結果格納用
	buffers_[static_cast<uint8_t>(Layout::Indirect_Reservoir_Diffuse)]  = std::make_unique<FBaseTexture>(downsize_, GetFormat(Layout::Indirect_Reservoir_Diffuse));
	buffers_[static_cast<uint8_t>(Layout::Indirect_Reservoir_Specular)] = std::make_unique<FBaseTexture>(downsize_, GetFormat(Layout::Indirect_Reservoir_Specular));
	buffers_[static_cast<uint8_t>(Layout::Indirect_Moment)]             = std::make_unique<FBaseTexture>(downsize_, GetFormat(Layout::Indirect_Moment));

	atlas_ = GetAtlasSize(config_.maxSampleCount);

	//!< Indirect Atlas
	buffers_[static_cast<uint8_t>(Layout::Indirect_Atlas_Diffuse)]  = std::make_unique<FBaseTexture>(downsize_ * atlas_, GetFormat(Layout::Indirect_Atlas_Diffuse));
	buffers_[static_cast<uint8_t>(Layout::Indirect_Atlas_Specular)] = std::make_unique<FBaseTexture>(downsize_ * atlas_, GetFormat(Layout::Indirect_Atlas_Specular));
	
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

FBaseTexture* FLightingGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[static_cast<size_t>(layout)].get();
}

DXGI_FORMAT FLightingGBuffer::GetFormat(Layout layout) {
	return kFormats_[static_cast<size_t>(layout)];
}

uint32_t FLightingGBuffer::GetAtlasSize(uint32_t sampleCount) {
	uint32_t r = static_cast<uint32_t>(std::sqrt(sampleCount));
	Exception::Assert(sampleCount == r * r, "sample count invalid value.", "[max sample count ^ 0.5]");
	//!< [max sample count ^ 0.5]の値が整数になる必要がある

	return r;

}
