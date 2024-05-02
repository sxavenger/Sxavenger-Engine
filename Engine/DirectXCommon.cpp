#include "DirectXCommon.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <TextureManager.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXCommon class
////////////////////////////////////////////////////////////////////////////////////////////
DirectXCommon::DirectXCommon() {
}

DirectXCommon::~DirectXCommon() {
}

void DirectXCommon::Init(WinApp* winApp, int32_t clientWidth, int32_t clientHeight) {
	// DirectXObjectの初期化
	devices_         = std::make_unique<DxObject::Devices>();
	command_         = std::make_unique<DxObject::Command>(devices_.get());
	descriptorHeaps_ = std::make_unique<DxObject::DescriptorHeaps>(devices_.get());
	swapChains_      = std::make_unique<DxObject::SwapChain>(devices_.get(), command_.get(), descriptorHeaps_.get(), winApp, clientWidth, clientHeight);
	fences_          = std::make_unique<DxObject::Fence>(devices_.get());
	compilers_       = std::make_unique<DxObject::Compilers>();
	shaderTable_     = std::make_unique<DxObject::ShaderTable>();

	blendState_   = std::make_unique<DxObject::BlendState>();
	depthStencil_ = std::make_unique<DxObject::DepthStencil>(devices_.get(), descriptorHeaps_.get(), clientWidth, clientHeight);

	pipelineManager_ = std::make_unique<DxObject::PipelineManager>(devices_.get(), command_.get(), blendState_.get(), clientWidth, clientHeight);

	
}

void DirectXCommon::Term() {
	// DxObjectの解放
	pipelineManager_.reset();
	depthStencil_.reset();
	blendState_.reset();

	shaderTable_.reset();
	compilers_.reset();
	fences_.reset();
	swapChains_.reset();
	descriptorHeaps_.reset();
	command_.reset();
	devices_.reset();
}

void DirectXCommon::BeginOffscreen(Texture* dummyTexture) {
	assert(dummyTexture != nullptr && offscreenDummyTexture_ == nullptr);
	
	offscreenDummyTexture_ = dummyTexture;
	
	// コマンドリストの取得
	auto commandList = command_->GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.pResource   = offscreenDummyTexture_->GetResource();

	commandList->ResourceBarrier(
		1,
		&barrier
	);

	D3D12_CPU_DESCRIPTOR_HANDLE handle_RTV = offscreenDummyTexture_->GetRTVHandleCPU();

	commandList->OMSetRenderTargets(
		1,
		&handle_RTV,
		false,
		&depthStencil_->GetHandle()
	);

	// 画面のクリア
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList->ClearRenderTargetView(
		handle_RTV,
		clearColor,
		0, nullptr
	);

	// 深度をクリア
	commandList->ClearDepthStencilView(
		depthStencil_->GetHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0, 0, nullptr
	);
}

void DirectXCommon::EndOffscreen() {
	assert(offscreenDummyTexture_ != nullptr);

	// コマンドリストの取得
	auto commandList = command_->GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource   = offscreenDummyTexture_->GetResource();

	commandList->ResourceBarrier(
		1,
		&barrier
	);

	offscreenDummyTexture_ = nullptr;
}

void DirectXCommon::BeginFrame() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = command_->GetCommandList();

	// 書き込みバックバッファのインデックスを取得
	backBufferIndex_ = swapChains_->GetSwapChain()->GetCurrentBackBufferIndex();

	commandList->ResourceBarrier(
		1,
		swapChains_->GetTransitionBarrier(backBufferIndex_, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)
	);

	D3D12_CPU_DESCRIPTOR_HANDLE handle_RTV = swapChains_->GetHandleCPU_RTV(backBufferIndex_); 

	commandList->OMSetRenderTargets(
		1,
		&handle_RTV,
		false,
		&depthStencil_->GetHandle()
	);

	// 画面のクリア
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList->ClearRenderTargetView(
		handle_RTV,
		clearColor,
		0, nullptr
	);

	// 深度をクリア
	commandList->ClearDepthStencilView(
		depthStencil_->GetHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0, 0, nullptr
	);
}

void DirectXCommon::EndFrame() {
	command_->GetCommandList()->ResourceBarrier(
		1,
		swapChains_->GetTransitionBarrier(backBufferIndex_, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)
	);

	command_->Close();

	swapChains_->Present(1, 0);

	// GPUにシグナルを送る
	fences_->AddFenceValue();

	command_->Signal(fences_.get());

	fences_->WaitGPU();

	command_->Reset();
}

void DirectXCommon::Sent() {
	command_->Close();

	// GPUにシグナルを送る
	fences_->AddFenceValue();

	command_->Signal(fences_.get());

	fences_->WaitGPU();

	command_->Reset();
}

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}
