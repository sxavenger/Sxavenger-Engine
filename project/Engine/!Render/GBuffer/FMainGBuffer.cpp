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

void FMainGBuffer::TransitionBeginRenderTarget(const DirectXQueueContext* context, Layout layout, const D3D12_CPU_DESCRIPTOR_HANDLE& handleDSV) {

	auto commandList = context->GetCommandList();

	buffers_[static_cast<size_t>(layout)]->TransitionBeginRenderTarget(context);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = buffers_[static_cast<size_t>(layout)]->GetCPUHandleRTV();

	commandList->OMSetRenderTargets(
		1, &handle, false,
		&handleDSV
	);
}

void FMainGBuffer::TransitionEndRenderTarget(const DirectXQueueContext* context, Layout layout) {
	buffers_[static_cast<size_t>(layout)]->TransitionEndRenderTarget(context);
}

void FMainGBuffer::ClearRenderTarget(const DirectXQueueContext* context, Layout layout) {
	buffers_[static_cast<size_t>(layout)]->ClearRenderTarget(context);
}

void FMainGBuffer::TransitionBeginUnordered(const DirectXQueueContext* context, Layout layout) {
	buffers_[static_cast<size_t>(layout)]->TransitionBeginUnordered(context);
}

void FMainGBuffer::TransitionEndUnordered(const DirectXQueueContext* context, Layout layout) {
	buffers_[static_cast<size_t>(layout)]->TransitionEndUnordered(context);
}

FBaseTexture* FMainGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[static_cast<size_t>(layout)].get();
}

DXGI_FORMAT FMainGBuffer::GetFormat(Layout layout) {
	return kFormats[static_cast<size_t>(layout)];
}
