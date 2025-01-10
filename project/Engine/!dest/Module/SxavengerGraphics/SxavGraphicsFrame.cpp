#include "SxavGraphicsFrame.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Module/Camera/Camera3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SystematicRenderFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavGraphicsFrame::Create(const Vector2ui& size, SxavGraphicsFrameType type) {

	type_ = type;

	systematic_ = std::make_unique<SystematicRenderFrame>();
	systematic_->Create(size);

	xclipse_ = std::make_unique<XclipseProcessFrame>();
	xclipse_->Create(size);

	adaptive_ = std::make_unique<AdaptiveRenderFrame>();
	adaptive_->Create(size);

	visual_ = std::make_unique<VisualProcessFrame>();
	visual_->Create(size);

	//* depth

	depthBuffer_ = std::make_unique<DepthBufferController>();
	depthBuffer_->Create(size);

	//* buffer

	config_ = std::make_unique<DimensionBuffer<Config>>();
	config_->Create(SxavengerSystem::GetDxDevice(), 1);
	(*config_)[0].size = size;
}

void SxavGraphicsFrame::Term() {
}

void SxavGraphicsFrame::BeginSystematic(const DirectXThreadContext* context) {
	systematic_->TransitionBeginRenderTarget(context);
	depthBuffer_->TransitionBeginRasterizeDepthWrite(context);

	// RenderTargetとDepthStencilを設定
	SystematicRenderFrame::GBufferArray<D3D12_CPU_DESCRIPTOR_HANDLE> handles;

	for (uint8_t i = 0; i < handles.size(); ++i) {
		handles[i] = systematic_->GetCPUHandleRTV(i);
	}

	context->GetCommandList()->OMSetRenderTargets(
		static_cast<UINT>(handles.size()), handles.data(),
		false,
		&depthBuffer_->GetRasterizerCPUHandleDSV()
	);
}

void SxavGraphicsFrame::EndSystematic(const DirectXThreadContext* context) {
	systematic_->TransitionEndRenderTarget(context);
	depthBuffer_->TransitionEndRasterizeDepthWrite(context);
}

void SxavGraphicsFrame::ClearSystematic(const DirectXThreadContext* context) {
	systematic_->ClearRenderTarget(context);
}

void SxavGraphicsFrame::BeginAdaptive(const DirectXThreadContext* context) {
	adaptive_->TransitionBeginRenderTarget(context);
	depthBuffer_->TransitionBeginRasterizeDepthWrite(context);

	context->GetCommandList()->OMSetRenderTargets(
		1, &adaptive_->GetCPUHandleRTV(),
		false,
		&depthBuffer_->GetRasterizerCPUHandleDSV()
	);
}

void SxavGraphicsFrame::EndAdaptive(const DirectXThreadContext* context) {
	adaptive_->TransitionEndRenderTarget(context);
	depthBuffer_->TransitionEndRasterizeDepthWrite(context);
}

void SxavGraphicsFrame::ClearRasterizerDepth(const DirectXThreadContext* context) {
	depthBuffer_->ClearRasterizeDepth(context);
}

void SxavGraphicsFrame::BeginXclipse(const DirectXThreadContext* context) {
	xclipse_->TransitionBeginProcess(context);
}

void SxavGraphicsFrame::EndXclipse(const DirectXThreadContext* context) {
	xclipse_->TransitionEndProcess(context);
}

void SxavGraphicsFrame::BeginVisual(const DirectXThreadContext* context) {
	visual_->TransitionBeginProcess(context);
}

void SxavGraphicsFrame::EndVisual(const DirectXThreadContext* context) {
	visual_->TransitionEndProcess(context);
}

void SxavGraphicsFrame::TransitionXclipseToAdaptive(const DirectXThreadContext* context) {
	CopyTexture(context, adaptive_->GetTexture(), xclipse_->GetResultBuffer());
}

void SxavGraphicsFrame::TransitionAdaptiveToVisual(const DirectXThreadContext* context) {
	visual_->ResetResultBufferIndex();
	CopyTexture(context, visual_->GetResultBuffer(), adaptive_->GetTexture());
}

void SxavGraphicsFrame::TransitionVisualToAdaptive(const DirectXThreadContext* context) {
	CopyTexture(context, adaptive_->GetTexture(), visual_->GetResultBuffer());
}

DxObject::BindBufferDesc SxavGraphicsFrame::GetTransitionSystematicBindDesc() const {
	DxObject::BindBufferDesc bind = {};
	bind.SetHandle("gAlbedo",   systematic_->GetGPUHandleSRV(SystematicRenderFrame::GBuffer::kAlbedo));
	bind.SetHandle("gNormal",   systematic_->GetGPUHandleSRV(SystematicRenderFrame::GBuffer::kNormal));
	bind.SetHandle("gPosition", systematic_->GetGPUHandleSRV(SystematicRenderFrame::GBuffer::kPosition));
	bind.SetAddress("gCamera",  camera_->GetGPUVirtualAddress());
	bind.SetAddress("gConfig",  config_->GetGPUVirtualAddress());
	bind.SetHandle("gXclipse",  xclipse_->GetResultBuffer()->GetGPUHandleUAV());
	bind.SetAddress("gCamera",  camera_->GetGPUVirtualAddress());

	return bind;
}

void SxavGraphicsFrame::CopyTexture(const DirectXThreadContext* context, const MultiViewTextureBuffer* dst, const MultiViewTextureBuffer* src) {

	auto commandList = context->GetCommandList();

	ID3D12Resource* const resourceSrc = src->GetResource();
	ID3D12Resource* const resourceDst = dst->GetResource();

	//* 共通情報の設定
	D3D12_RESOURCE_BARRIER barriers[2] = {};
	barriers[0].Type                 = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Transition.pResource = resourceSrc;
	barriers[1].Type                 = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[1].Transition.pResource = resourceDst;

	//* copy可能状態に遷移
	// src
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE;

	// dst
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
	

	commandList->ResourceBarrier(_countof(barriers), barriers);

	commandList->CopyResource(resourceDst, resourceSrc);

	//* 元の状態に遷移
	// src
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barriers[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	// dst
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barriers[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	commandList->ResourceBarrier(_countof(barriers), barriers);

}


