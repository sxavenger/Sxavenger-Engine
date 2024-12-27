#include "DirectXContext.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXThreadContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXThreadContext::Init(uint32_t allocatorCount) {
	command_ = std::make_unique<CommandContext>();
	command_->Init(SxavengerSystem::GetDxDevice(), allocatorCount);
	SetDescriptorHeap();
}

void DirectXThreadContext::Term() {
}

void DirectXThreadContext::TransitionAllocator() const {
	command_->TransitionAllocator();
	SetDescriptorHeap();
}

void DirectXThreadContext::ExecuteAllAllocators() const {
	command_->ExecuteAllAllocators();
	SetDescriptorHeap();
}

ID3D12GraphicsCommandList6* DirectXThreadContext::GetCommandList() const {
	return command_->GetCommandList();
}

ID3D12CommandQueue* DirectXThreadContext::GetCommandQueue() const {
	return command_->GetCommandQueue();
}

void DirectXThreadContext::SetDescriptorHeap() const {

	ID3D12DescriptorHeap* descriptorHeaps[] = {
		SxavengerSystem::GetDxDescriptorHeaps()->GetDescriptorHeap(kDescriptor_CBV_SRV_UAV)
	};

	command_->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXWindowContext class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXWindowContext::Init(Window* window, DirectXThreadContext* threadContext, const Color4f& clearColor) {

	// 引数の保存
	threadContext_ = threadContext;

	swapChain_ = std::make_unique<SwapChain>();
	swapChain_->Init(
		SxavengerSystem::GetDxDevice(), SxavengerSystem::GetDxDescriptorHeaps(), threadContext_->GetDxCommand(),
		window
	);

	depthStencil_ = std::make_unique<DepthStencil>();
	depthStencil_->Init(SxavengerSystem::GetDxDevice(), SxavengerSystem::GetDxDescriptorHeaps(), window->GetSize());

	clearColor_ = clearColor;
}

void DirectXWindowContext::Term() {
}

void DirectXWindowContext::BeginRendering() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = threadContext_->GetCommandList();

	// 書き込みバックバッファのインデックスを取得
	swapChain_->GetCurrentBackBufferIndex();

	// backBufferを書き込み状態に変更
	D3D12_RESOURCE_BARRIER barrier = swapChain_->GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	commandList->ResourceBarrier(1, &barrier);

	// RTV, DSVの設定
	commandList->OMSetRenderTargets(
		1, &swapChain_->GetBackBufferCPUHandle(),
		false,
		&depthStencil_->GetCPUHandle()
	);
}

void DirectXWindowContext::EndRendering() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = threadContext_->GetCommandList();

	// backBufferをpresent状態に変更
	D3D12_RESOURCE_BARRIER barrier = swapChain_->GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	commandList->ResourceBarrier(1, &barrier);

}

void DirectXWindowContext::ClearWindow(bool isClearColor, bool isClearDepth) {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = threadContext_->GetCommandList();

	if (isClearColor) {
		// 画面のクリア
		commandList->ClearRenderTargetView(
			swapChain_->GetBackBufferCPUHandle(),
			&clearColor_.r,
			0, nullptr
		);
	}

	if (isClearDepth) {
		// 深度をクリア
		commandList->ClearDepthStencilView(
			depthStencil_->GetCPUHandle(),
			D3D12_CLEAR_FLAG_DEPTH,
			1.0f,
			0, 0, nullptr
		);
	}

	isClearWindow_ = true;
}

void DirectXWindowContext::TryClearWindow(bool isClearColor, bool isClearDepth) {
	if (isClearWindow_) {
		return;
	}

	ClearWindow(isClearColor, isClearDepth);
}

void DirectXWindowContext::Present(bool isTransitonAllocator) {
	if (isTransitonAllocator) {
		threadContext_->TransitionAllocator();
	}
	swapChain_->Present(1, 0);

	isClearWindow_ = false;
}
