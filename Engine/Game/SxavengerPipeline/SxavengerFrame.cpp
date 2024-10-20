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

void XclipseProcessFrame::BeginUnorderedAccess() {

	auto commandList = Sxavenger::GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.pResource   = buffer_->GetResource();

	commandList->ResourceBarrier(1, &barrier);
}

void XclipseProcessFrame::EndUnorderedAccess() {

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

////////////////////////////////////////////////////////////////////////////////////////////
// VisualProcessFrame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void VisualProcessFrame::Create(const Vector2ui& size) {
	for (uint32_t i = 0; i < kProcessBufferNum_; ++i) {
		buffers_[i] = std::make_unique<MultiViewTexture>();
		buffers_[i]->Create(VIEWFLAG_UAV, size, {}, kOffscreenFormat);
	}
}

void VisualProcessFrame::Term() {
}

const D3D12_GPU_DESCRIPTOR_HANDLE& VisualProcessFrame::GetReferenceTextureHandle(uint32_t prevIndex) {
	Assert(prevIndex >= kProcessBufferNum_, "prevIndex must not over process buffer num");

	uint32_t index = (resultBufferIndex_ + kProcessBufferNum_ - prevIndex) % kProcessBufferNum_;
	return buffers_.at(index)->GetGPUHandleSRV();
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

	//* depth texture *//

	depthStencilTexture_ = std::make_unique<DepthStencilTexture>();
	depthStencilTexture_->Create(size);

	//* config buffer

	config_ = std::make_unique<BufferResource<SxavengerFrameConfig>>(Sxavenger::GetDevicesObj(), 1);
	(*config_)[0].size = size;
}

void SxavengerFrame::Term() {
}

void SxavengerFrame::BeginSystematic(bool isDepthClear, bool isRenderTargetClear) {
	depthStencilTexture_->BeginDepthWrite(isDepthClear);
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
		&depthStencilTexture_->GetCPUHandleDSV()
	);
}

void SxavengerFrame::EndSystematic() {
	depthStencilTexture_->EndDepthWrite();
	systematic_->EndRenderingBuffer();
}

void SxavengerFrame::TransitionSystematicToXclipse() {

	auto commandList = Sxavenger::GetCommandList();

	xclipse_->BeginUnorderedAccess();

	sSystemConsole->GetProcessConsole()->SetPipeline(kTransition_SampleLighting);

	commandList->SetComputeRootDescriptorTable(0, systematic_->GetBuffer(SystematicRenderingFrame::GBUFFER_ALBEDO)->GetGPUHandleSRV());
	commandList->SetComputeRootDescriptorTable(1, systematic_->GetBuffer(SystematicRenderingFrame::GBUFFER_NORMAL)->GetGPUHandleSRV());
	commandList->SetComputeRootDescriptorTable(2, systematic_->GetBuffer(SystematicRenderingFrame::GBUFFER_POSITION)->GetGPUHandleSRV());
	commandList->SetComputeRootConstantBufferView(3, config_->GetGPUVirtualAddress());
	commandList->SetComputeRootDescriptorTable(4, xclipse_->GetBuffer()->GetGPUHandleUAV());

	sSystemConsole->GetProcessConsole()->Dispatch((*config_)[0].size);

	xclipse_->EndUnorderedAccess();
}

void SxavengerFrame::TransitionXclipseToAdaptive() {

	auto commandList = Sxavenger::GetCommandList();



}

void SxavengerFrame::TransitionAdaptiveToVisual() {
}

void SxavengerFrame::TransitionVisualToAdaptive() {
}

void SxavengerFrame::PresentAdaptiveToScreen() {
}

void SxavengerFrame::PresentVisualToScreen() {
}
