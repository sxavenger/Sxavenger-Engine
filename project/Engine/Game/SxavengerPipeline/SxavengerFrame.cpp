#include "SxavengerFrame.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Console/SystemConsole.h>

//=========================================================================================
// SystematicRenderingFrame class static variables
//=========================================================================================

const SystematicRenderingFrame::GBufferArray<DXGI_FORMAT> SystematicRenderingFrame::formats_ = {
	kOffscreenFormat,               //!< albedo
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< normal
	DXGI_FORMAT_R32G32B32A32_FLOAT, //!< position
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< material
};

////////////////////////////////////////////////////////////////////////////////////////////
// SystematicRenderingFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SystematicRenderingFrame::Create(const Vector2ui& size) {
	for (uint32_t i = 0; i < GBuffer::kCountOfGBuffer; ++i) {
		buffers_[i] = std::make_unique<MultiViewTexture>();
		buffers_[i]->Create(VIEWFLAG_RENDER_TEXTURE, size, {}, formats_[i]);
	}
}

void SystematicRenderingFrame::Term() {
}

void SystematicRenderingFrame::BeginRenderingBuffer(bool isClearRenderTerget) {

	auto commandList = Sxavenger::GetCommandList();

	// barrierの設定
	GBufferArray<D3D12_RESOURCE_BARRIER> barriers = {};

	for (uint32_t i = 0; i < GBuffer::kCountOfGBuffer; ++i) {
		barriers[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[i].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barriers[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barriers[i].Transition.pResource   = buffers_[i]->GetResource();
	}

	commandList->ResourceBarrier(GBuffer::kCountOfGBuffer, barriers.data());

	if (!isClearRenderTerget) {
		return;
	}

	for (uint32_t i = 0; i < GBuffer::kCountOfGBuffer; ++i) {
		commandList->ClearRenderTargetView(
			buffers_[i]->GetCPUHandleRTV(),
			&buffers_[i]->GetClearColor().r,
			0, nullptr
		);
	}
}

void SystematicRenderingFrame::EndRenderingBuffer() {

	auto commandList = Sxavenger::GetCommandList();

	// barrierの設定
	GBufferArray<D3D12_RESOURCE_BARRIER> barriers = {};

	for (uint32_t i = 0; i < GBuffer::kCountOfGBuffer; ++i) {
		barriers[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[i].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barriers[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barriers[i].Transition.pResource   = buffers_[i]->GetResource();
	}

	commandList->ResourceBarrier(GBuffer::kCountOfGBuffer, barriers.data());
}

////////////////////////////////////////////////////////////////////////////////////////////
// XclipseProcessFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void XclipseProcessFrame::Create(const Vector2ui& size) {
	buffer_ = std::make_unique<MultiViewTexture>();
	buffer_->Create(VIEWFLAG_UAV, size, {}, kOffscreenFormat);
}

void XclipseProcessFrame::Term() {
}

void XclipseProcessFrame::BeginProcess() {

	auto commandList = Sxavenger::GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.pResource   = buffer_->GetResource();

	commandList->ResourceBarrier(1, &barrier);
}

void XclipseProcessFrame::EndProcess() {

	auto commandList = Sxavenger::GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource   = buffer_->GetResource();

	commandList->ResourceBarrier(1, &barrier);
}

////////////////////////////////////////////////////////////////////////////////////////////
// AdaptiveRenderingFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AdaptiveRenderingFrame::Create(const Vector2ui& size) {
	buffer_ = std::make_unique<MultiViewTexture>();
	buffer_->Create(VIEWFLAG_RENDER_TEXTURE, size, {}, kOffscreenFormat);
}

void AdaptiveRenderingFrame::Term() {
}

void AdaptiveRenderingFrame::BeginRenderingBuffer() {

	auto commandList = Sxavenger::GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.pResource   = buffer_->GetResource();

	commandList->ResourceBarrier(1, &barrier);
}

void AdaptiveRenderingFrame::EndRenderingBuffer() {

	auto commandList = Sxavenger::GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource   = buffer_->GetResource();

	commandList->ResourceBarrier(1, &barrier);
}

////////////////////////////////////////////////////////////////////////////////////////////
// VisualProcessFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void VisualProcessFrame::Create(const Vector2ui& size) {
	for (uint32_t i = 0; i < kProcessBufferNum_; ++i) {
		buffers_[i] = std::make_unique<MultiViewTexture>();
		buffers_[i]->Create(VIEWFLAG_UAV | VIEWFLAG_SRV, size, {}, kOffscreenFormat);
	}
}

void VisualProcessFrame::Term() {
}

void VisualProcessFrame::BeginProcess() {

	auto commandList = Sxavenger::GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, kProcessBufferNum_> barrires = {};

	for (uint32_t i = 0; i < kProcessBufferNum_; ++i) {
		barrires[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrires[i].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrires[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrires[i].Transition.pResource   = buffers_[i]->GetResource();
	}

	commandList->ResourceBarrier(kProcessBufferNum_, barrires.data());
}

void VisualProcessFrame::EndProcess() {

	auto commandList = Sxavenger::GetCommandList();

	std::array<D3D12_RESOURCE_BARRIER, kProcessBufferNum_> barrires = {};

	for (uint32_t i = 0; i < kProcessBufferNum_; ++i) {
		barrires[i].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrires[i].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrires[i].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrires[i].Transition.pResource   = buffers_[i]->GetResource();
	}

	commandList->ResourceBarrier(kProcessBufferNum_, barrires.data());
}

void VisualProcessFrame::NextResultBufferIndex() {
	resultBufferIndex_++;
	resultBufferIndex_ %= kProcessBufferNum_;
}

void VisualProcessFrame::ResetResultBufferIndex() {
	resultBufferIndex_ = 0;
}

MultiViewTexture* VisualProcessFrame::GetPrevBuffer(uint32_t prev) const {
	Assert(prev < kProcessBufferNum_, "visual: prev over process buffer.");

	uint32_t index = (resultBufferIndex_ + kProcessBufferNum_ - prev) % kProcessBufferNum_;
	return buffers_.at(index).get();
}

////////////////////////////////////////////////////////////////////////////////////////////
// ScreenPresenter class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ScreenPresenter::Init() {
	ia_ = std::make_unique<BufferResource<ScreenPresentIA>>(Sxavenger::GetDevicesObj(), 3);
	(*ia_)[0].position = { -1.0f, 1.0f, 1.0f };
	(*ia_)[0].texcoord = { 0.0f, 0.0f };

	(*ia_)[1].position = { 3.0f, 1.0f, 1.0f };
	(*ia_)[1].texcoord = { 2.0f, 0.0f };

	(*ia_)[2].position = { -1.0f, -3.0f, 1.0f };
	(*ia_)[2].texcoord = { 0.0f, 2.0f };
}

void ScreenPresenter::Term() {
}

void ScreenPresenter::Present(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {

	auto commandList = Sxavenger::GetCommandList();

	sSystemConsole->SetRenderingPipeline(kPresentToScreen);

	D3D12_VERTEX_BUFFER_VIEW vbv = ia_->GetVertexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->SetGraphicsRootDescriptorTable(0, handle);

	commandList->DrawInstanced(ia_->GetIndexSize(), 1, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerFrame::Create(const Vector2ui& size) {

	//* sxavenger frame

	systematic_ = std::make_unique<SystematicRenderingFrame>();
	systematic_->Create(size);

	xclipse_ = std::make_unique<XclipseProcessFrame>();
	xclipse_->Create(size);

	adaptive_ = std::make_unique<AdaptiveRenderingFrame>();
	adaptive_->Create(size);

	visual_ = std::make_unique<VisualProcessFrame>();
	visual_->Create(size);

	//* depth buffer

	depthBuffer_ = std::make_unique<DepthBufferContoller>();
	depthBuffer_->Create(size);

	//* config buffer

	config_ = std::make_unique<BufferResource<SxavengerFrameConfig>>(Sxavenger::GetDevicesObj(), 1);
	(*config_)[0].size = size;

	//* presenter

	presenter_ = std::make_unique<ScreenPresenter>();
	presenter_->Init();
}

void SxavengerFrame::Term() {
}

void SxavengerFrame::BeginSystematic(bool isDepthClear, bool isRenderTargetClear) {
	depthBuffer_->BeginRasterizeDepthWrite(isDepthClear);
	systematic_->BeginRenderingBuffer(isRenderTargetClear);

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	// RenderTargetとDepthStencilを設定
	SystematicRenderingFrame::GBufferArray<D3D12_CPU_DESCRIPTOR_HANDLE> handles;

	for (uint32_t i = 0; i < SystematicRenderingFrame::GBuffer::kCountOfGBuffer; ++i) {
		handles[i] = systematic_->GetBuffers()[i]->GetCPUHandleRTV();
	}

	commandList->OMSetRenderTargets(
		SystematicRenderingFrame::GBuffer::kCountOfGBuffer, handles.data(),
		false,
		&depthBuffer_->GetRasterizeCPUHandleDSV()
	);
}

void SxavengerFrame::EndSystematic() {
	depthBuffer_->EndRasterizeDepthWrite();
	systematic_->EndRenderingBuffer();
}

void SxavengerFrame::BeginAdaptive(bool isDepthClear) {
	depthBuffer_->BeginRasterizeDepthWrite(isDepthClear);
	adaptive_->BeginRenderingBuffer();

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	commandList->OMSetRenderTargets(
		1, &adaptive_->GetTexture()->GetCPUHandleRTV(),
		false,
		&depthBuffer_->GetRasterizeCPUHandleDSV()
	);
}

void SxavengerFrame::EndAdaptive() {
	depthBuffer_->EndRasterizeDepthWrite();
	adaptive_->EndRenderingBuffer();
}

void SxavengerFrame::BeginRaytracing() {
	depthBuffer_->BeginRaytracingDepthWrite();
	xclipse_->BeginProcess();
}

void SxavengerFrame::EndRaytracing() {
	depthBuffer_->EndRaytracingDepthWrite();
	xclipse_->EndProcess();
}

void SxavengerFrame::BeginXclipse() {
	xclipse_->BeginProcess();
}

void SxavengerFrame::EndXclipse() {
	xclipse_->EndProcess();
}

void SxavengerFrame::BeginVisual() {
	visual_->BeginProcess();
}

void SxavengerFrame::EndVisual() {
	visual_->EndProcess();
}

void SxavengerFrame::TransitionSystematicToXclipse() {

	auto commandList = Sxavenger::GetCommandList();

	xclipse_->BeginProcess();

	sSystemConsole->SetProcessPipeline(kTransition_SampleLighting);

	commandList->SetComputeRootDescriptorTable(0, systematic_->GetTexture(SystematicRenderingFrame::GBUFFER_ALBEDO)->GetGPUHandleSRV());
	commandList->SetComputeRootDescriptorTable(1, systematic_->GetTexture(SystematicRenderingFrame::GBUFFER_NORMAL)->GetGPUHandleSRV());
	commandList->SetComputeRootDescriptorTable(2, systematic_->GetTexture(SystematicRenderingFrame::GBUFFER_POSITION)->GetGPUHandleSRV());
	commandList->SetComputeRootDescriptorTable(3, systematic_->GetTexture(SystematicRenderingFrame::GBUFFER_MATERIAL)->GetGPUHandleSRV());
	commandList->SetComputeRootConstantBufferView(4, config_->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(5, camera_->GetGPUVirtualAddress());
	commandList->SetComputeRootDescriptorTable(6, xclipse_->GetTexture()->GetGPUHandleUAV());

	sSystemConsole->Dispatch((*config_)[0].size);

	xclipse_->EndProcess();
}

void SxavengerFrame::TransitionXclipseToAdaptive() {
	CopyTexture(adaptive_->GetTexture(), xclipse_->GetTexture());
}

void SxavengerFrame::TransitionAdaptiveToVisual() {
	visual_->ResetResultBufferIndex();
	CopyTexture(visual_->GetResultBuffer(), adaptive_->GetTexture());  
}

void SxavengerFrame::TransitionVisualToAdaptive() {
	CopyTexture(adaptive_->GetTexture(), visual_->GetResultBuffer());
}

void SxavengerFrame::TransitionRaytracingToRasterize() {
	depthBuffer_->TransferRaytracingToRasterize();
}

void SxavengerFrame::PresentAdaptiveToScreen() {
	presenter_->Present(adaptive_->GetTexture()->GetGPUHandleSRV());
}

void SxavengerFrame::CopyTexture(const MultiViewTexture* dst, const MultiViewTexture* src) {

	auto commandList = Sxavenger::GetCommandList();

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
