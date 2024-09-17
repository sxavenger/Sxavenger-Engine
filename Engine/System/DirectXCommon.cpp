#include "DirectXCommon.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <imgui.h>

//* forward
#include "Texture.h"
#include <Engine/Beta/DepthRenderTarget.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXCommon class
////////////////////////////////////////////////////////////////////////////////////////////

void DirectXCommon::Init(WinApp* winApp) {
	// DirectXObjectの初期化
	// system menbers
	devices_         = std::make_unique<DxObject::Devices>();
	command_         = std::make_unique<DxObject::Command>(devices_.get());
	descriptorHeaps_ = std::make_unique<DxObject::DescriptorHeaps>(devices_.get());
	swapChains_      = std::make_unique<DxObject::SwapChain>(devices_.get(), command_.get(), descriptorHeaps_.get(), winApp);
	fences_          = std::make_unique<DxObject::Fence>(devices_.get());
	
	// new Graphics pipeline menbers
	shaderManager_ = std::make_unique<DxObject::ShaderBlobManager>();

	blendState_   = std::make_unique<DxObject::BlendState>();
	depthStencil_ = std::make_unique<DxObject::DepthStencil>(devices_.get(), descriptorHeaps_.get(), winApp);
}

void DirectXCommon::Term() {
	// DxObjectの解放
}

void DirectXCommon::EndFrame() {

	EndRendering();

	command_->GetCommandList()->ResourceBarrier(
		1,
		swapChains_->GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)
	);

	fences_->WaitGPU(); // frontAllocatorの完了待ち

	command_->Close();

	swapChains_->Present(1, 0);

	// GPUにシグナルを送る
	fences_->AddFenceValue();

	command_->Signal(fences_.get());

	fences_->WaitGPU();

	command_->ResetAllAllocator();

}

void DirectXCommon::BeginOffscreen(RenderTexture* renderTexture) {

	Assert(renderTexture != nullptr);

	BeginRendering();
	
	// コマンドリストの取得
	auto commandList = command_->GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.pResource   = renderTexture->GetResource();

	commandList->ResourceBarrier(
		1,
		&barrier
	);

	commandList->OMSetRenderTargets(
		1,
		&renderTexture->GetCPUHandleRTV(),
		false,
		&depthStencil_->GetHandle()
	);

	// 画面のクリア
	commandList->ClearRenderTargetView(
		renderTexture->GetCPUHandleRTV(),
		&renderTexture->GetClearColor().r,
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

void DirectXCommon::EndOffscreen(RenderTexture* renderTexture) {

	EndRendering();

	// コマンドリストの取得
	auto commandList = command_->GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource   = renderTexture->GetResource();

	commandList->ResourceBarrier(
		1, &barrier
	);
}

void DirectXCommon::BeginOffscreens(uint32_t textureNum, RenderTexture* renderTextures[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]) {

	BeginRendering();

	// コマンドリストの取得
	auto commandList = command_->GetCommandList();
	
	std::vector<D3D12_RESOURCE_BARRIER>      barriers;                //!< barrierの配列
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetDescriptors; //!< RTVのCPUHandle配列
	
	for (uint32_t i = 0; i < textureNum; ++i) {

		// barrierの設定
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.pResource   = renderTextures[i]->GetResource();

		barriers.push_back(barrier);

		// CPUHandleの保存
		renderTargetDescriptors.push_back(renderTextures[i]->GetCPUHandleRTV());
	}

	Assert(barriers.size() != 0); //!< renderingするtextureが存在しない

	// renderTexturesの遷移
	commandList->ResourceBarrier(
		static_cast<UINT>(barriers.size()), barriers.data()
	);

	// renderTexturesをRenderTargetに設定
	commandList->OMSetRenderTargets(
		static_cast<UINT>(renderTargetDescriptors.size()), renderTargetDescriptors.data(),
		false,
		&depthStencil_->GetHandle()
	);

	// 画面のクリア
	for (uint32_t i = 0; i < static_cast<uint32_t>(renderTargetDescriptors.size()); ++i) {

		commandList->ClearRenderTargetView(
			renderTargetDescriptors[i],
			&renderTextures[i]->GetClearColor().r,
			0, nullptr
		);
	}

	// 深度をクリア
	commandList->ClearDepthStencilView(
		depthStencil_->GetHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0, 0, nullptr
	);
}

void DirectXCommon::EndOffscreens(uint32_t textureNum, RenderTexture* renderTextures[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]) {

	EndRendering();

	// コマンドリストの取得
	auto commandList = command_->GetCommandList();

	std::vector<D3D12_RESOURCE_BARRIER> barriers; //!< barrierの配列
	
	for (uint32_t i = 0; i < textureNum; ++i) {

		// barrierの設定
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.pResource   = renderTextures[i]->GetResource();

		barriers.push_back(barrier);
	}

	// renderTexturesの遷移
	commandList->ResourceBarrier(
		static_cast<UINT>(barriers.size()), barriers.data()
	);
}

void DirectXCommon::BeginOffScreen(DepthRenderTarget* depthRenderTarget, bool isClearRenderTarget) {

	BeginRendering();

	// コマンドリストの取得
	auto commandList = command_->GetCommandList();

	// RTVにbarrier
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.pResource   = depthRenderTarget->GetResource();

	commandList->ResourceBarrier(
		1, &barrier
	);

	// RTVの設定
	commandList->OMSetRenderTargets(
		1,
		&depthRenderTarget->GetCPUHandleRTV(),
		false,
		&depthRenderTarget->GetCPUHandleDSV()
	);

	if (isClearRenderTarget) {

		// 画面のクリア
		commandList->ClearRenderTargetView(
			depthRenderTarget->GetCPUHandleRTV(),
			&depthRenderTarget->GetClearColor().r,
			0, nullptr
		);

		// 深度をクリア
		commandList->ClearDepthStencilView(
			depthRenderTarget->GetCPUHandleDSV(),
			D3D12_CLEAR_FLAG_DEPTH,
			1.0f,
			0, 0, nullptr
		);

	}
}

void DirectXCommon::EndOffScreen(DepthRenderTarget* depthRenderTarget) {

	EndRendering();

	// コマンドリストの取得
	auto commandList = command_->GetCommandList();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.pResource   = depthRenderTarget->GetResource();

	commandList->ResourceBarrier(
		1, &barrier
	);
}

void DirectXCommon::BeginScreenDraw() {

	BeginRendering();

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = command_->GetCommandList();

	// 書き込みバックバッファのインデックスを取得
	swapChains_->ObtainCurrentBackBufferIndex();

	commandList->ResourceBarrier(
		1,
		swapChains_->GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)
	);

	commandList->OMSetRenderTargets(
		1,
		&swapChains_->GetRTVHandleCPU(),
		false,
		&depthStencil_->GetHandle()
	);

	// 画面のクリア
	commandList->ClearRenderTargetView(
		swapChains_->GetRTVHandleCPU(),
		&defaultClearColor.r,
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

void DirectXCommon::BeginUnorderedAccess(DummyTexture* dummyTexture) {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = command_->GetCommandList();

	// dummyTextureを書き込み状態に遷移
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource   = dummyTexture->GetResource();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

		commandList->ResourceBarrier(1, &barrier);
	}
}

void DirectXCommon::EndUnorderedAccess(DummyTexture* dummyTexture) {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = command_->GetCommandList();

	// dummyTextureを読み込み状態に遷移
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource   = dummyTexture->GetResource();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		commandList->ResourceBarrier(1, &barrier);
	}
}

void DirectXCommon::TranstionAllocator() {

	fences_->WaitGPU(); // frontAllocatorの完了待ち

	command_->Close();

	// GPUにシグナルを送る
	fences_->AddFenceValue();

	command_->Signal(fences_.get());

	command_->ResetBackAllocatorAndTranstion();

	ID3D12DescriptorHeap* srv[] = { descriptorHeaps_->GetDescriptorHeap(CBV_SRV_UAV) };
	command_->GetCommandList()->SetDescriptorHeaps(_countof(srv), srv);

}

void DirectXCommon::ResetBackAllocator() {
	fences_->WaitGPU(); // frontAllocatorの完了待ち
	//!< todo: frontAllocatorは使わないのでこのwaitをなしにする
	
	command_->Close();

	fences_->AddFenceValue();

	command_->Signal(fences_.get());

	fences_->WaitGPU();

	command_->ResetBackAllocator();

	ID3D12DescriptorHeap* srv[] = { descriptorHeaps_->GetDescriptorHeap(CBV_SRV_UAV) };
	command_->GetCommandList()->SetDescriptorHeaps(_countof(srv), srv);
}

void DirectXCommon::CopyResource(
	ID3D12Resource* dst, D3D12_RESOURCE_STATES dstState,
	ID3D12Resource* src, D3D12_RESOURCE_STATES srcState) {

	// commandListの取得
	auto commandList = command_->GetCommandList();

	// copyするため, 各Resourceのstateの変更
	{
		// dst
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource   = dst;
		barrier.Transition.StateBefore = dstState;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
		
		commandList->ResourceBarrier(1, &barrier);
	}

	{
		// src
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource   = src;
		barrier.Transition.StateBefore = srcState;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE;

		commandList->ResourceBarrier(1, &barrier);
	}

	// copyの実行
	commandList->CopyResource(dst, src);

	// 元のstate状態に戻す
	{
		// dst
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource   = dst;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter  = dstState;
		
		commandList->ResourceBarrier(1, &barrier);
	}

	{
		// src
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource   = src;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barrier.Transition.StateAfter  = srcState;

		commandList->ResourceBarrier(1, &barrier);
	}
}

void DirectXCommon::BeginRendering() {
	Assert(!isRendering_); //!< 他がrendering中
	isRendering_ = true;
}

void DirectXCommon::EndRendering() {
	isRendering_ = false;
}
