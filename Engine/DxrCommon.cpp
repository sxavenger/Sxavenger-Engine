#include "DxrCommon.h"

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxrObject;

////////////////////////////////////////////////////////////////////////////////////////////
// DxrCommon class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxrCommon::Init(WinApp* winApp, int32_t clientWidth, int32_t clientHeight) {
	devices_         = std::make_unique<Devices>();
	command_         = std::make_unique<Command>(devices_.get());
	descriptorHeaps_ = std::make_unique<DescriptorHeaps>(devices_.get());
	swapChain_       = std::make_unique<SwapChain>(
		devices_.get(), command_.get(), descriptorHeaps_.get(),
		winApp, clientWidth, clientHeight
	);
	fence_           = std::make_unique<Fence>(devices_.get());
}

void DxrCommon::Term() {
	fence_.reset();
	swapChain_.reset();
	descriptorHeaps_.reset();
	command_.reset();
	devices_.reset();
}

void DxrCommon::BeginFrame() {
	// コマンドリストの取得
	auto commandList = command_->GetCommandList();

	// 書き込みバックバッファのインデックスを取得
	backBufferIndex_ = swapChain_->GetSwapChain()->GetCurrentBackBufferIndex();

	commandList->ResourceBarrier(
		1,
		swapChain_->GetTransitionBarrier(backBufferIndex_, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)
	);

	D3D12_CPU_DESCRIPTOR_HANDLE handle_RTV = swapChain_->GetHandleCPU_RTV(backBufferIndex_);

	commandList->OMSetRenderTargets(
		1,
		&handle_RTV,
		false,
		nullptr
	);

	// 画面のクリア
	float clearColor[] = { 0.6f, 0.8f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(
		handle_RTV,
		clearColor,
		0, nullptr
	);
}

void DxrCommon::EndFrame() {
	command_->GetCommandList()->ResourceBarrier(
		1,
		swapChain_->GetTransitionBarrier(backBufferIndex_, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)
	);

	command_->Close();

	swapChain_->Present(1, 0);

	// GPUにシグナルを送る
	fence_->AddFenceValue();

	command_->Signal(fence_.get());

	fence_->WaitGPU();

	command_->Reset();
}

DxrCommon* DxrCommon::GetInstance() {
	static DxrCommon instance;
	return &instance;
}
