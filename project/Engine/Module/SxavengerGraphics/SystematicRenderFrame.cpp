#include "SystematicRenderFrame.h"
_DXOBJECT_USING

//=========================================================================================
// SystematicRenderingFrame class static variables
//=========================================================================================

const SystematicRenderFrame::GBufferArray<DXGI_FORMAT> SystematicRenderFrame::formats_ = {
	kOffscreenFormat,               //!< albedo
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< normal
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< position
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< material
};

////////////////////////////////////////////////////////////////////////////////////////////
// SystematicRenderFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SystematicRenderFrame::Create(const Vector2ui& size) {
	for (uint32_t i = 0; i < formats_.size(); ++i) {
		buffers_[i] = std::make_unique<MultiViewTexture>();
		buffers_[i]->Create(VIEWFLAG_RENDER_TEXTURE, size, {}, formats_[i]);
	}
}

void SystematicRenderFrame::Term() {
}

void SystematicRenderFrame::TransitionBeginRenderTarget(const DirectXThreadContext* context) {
	GBufferArray<D3D12_RESOURCE_BARRIER> barriers = {};

	for (uint32_t i = 0; i < formats_.size(); ++i) {
		barriers[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[i].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barriers[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barriers[i].Transition.pResource   = buffers_[i]->GetResource();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(formats_.size()), barriers.data());
}

void SystematicRenderFrame::TransitionEndRenderTarget(const DirectXThreadContext* context) {
	GBufferArray<D3D12_RESOURCE_BARRIER> barriers = {};

	for (uint32_t i = 0; i < formats_.size(); ++i) {
		barriers[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[i].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barriers[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barriers[i].Transition.pResource   = buffers_[i]->GetResource();
	}

	context->GetCommandList()->ResourceBarrier(static_cast<UINT>(formats_.size()), barriers.data());
}

void SystematicRenderFrame::ClearRenderTarget(const DirectXThreadContext* context) {
	for (uint32_t i = 0; i < formats_.size(); ++i) {
		buffers_[i]->ClearRenderTarget(context);
	}
}
