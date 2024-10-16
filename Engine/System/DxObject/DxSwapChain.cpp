#include "DxSwapChain.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// WinApp
#include <Engine/System/WinApp.h>

// DxObject
#include "DxDevices.h"
#include "DxCommand.h"
#include "DxDescriptorHeaps.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SwapChain methods
////////////////////////////////////////////////////////////////////////////////////////////

SwapChain::SwapChain(
	Devices* devices, Command* command, DescriptorHeaps* descriptorHeaps, WinApp* winApp) {

	Init(devices, command, descriptorHeaps, winApp);
}

SwapChain::~SwapChain() { Term(); }

void SwapChain::Init(
	Devices* devices, Command* command, DescriptorHeaps* descriptorHeaps, WinApp* winApp) {

	ID3D12Device* device = devices->GetDevice();

	// スワップチェインを生成
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width            = winApp->GetClientSize().x;
		desc.Height           = winApp->GetClientSize().y;
		desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount      = kBufferCount_;
		desc.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		auto hr = devices->GetFactory()->CreateSwapChainForHwnd(
			command->GetCommandQueue(),
			winApp->GetHwnd(),
			&desc,
			nullptr, nullptr,
			reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf())
		);

		Assert(SUCCEEDED(hr));
	}

	// SwapChainからResourceを引っ張ってくる
	{
		for (uint32_t i = 0; i < kBufferCount_; ++i) {
			auto hr = swapChain_->GetBuffer(
				i, IID_PPV_ARGS(&resource_[i])
			);

			Assert(SUCCEEDED(hr));
		}
	}

	// RTVの設定
	{
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format        = kScreenFormat;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		Assert(descriptorHeaps->GetDescriptorMaxCount(DescriptorType::RTV) >= kBufferCount_);

		for (uint32_t i = 0; i < kBufferCount_; ++i) {
			descriptorRTV_[i] = descriptorHeaps->GetDescriptor(DescriptorType::RTV);

			device->CreateRenderTargetView(
				resource_[i].Get(), &desc, descriptorRTV_[i].GetCPUHandle()
			);
		}
	}

	// barrierの設定
	{
		barrier_.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	}

	Log("[_DXOBJECT]::SwapChain complete init.");
}

void SwapChain::Term() {
}

void SwapChain::Present(UINT SyncInterval, UINT Flags) {
	swapChain_->Present(SyncInterval, Flags);
}

void SwapChain::ObtainCurrentBackBufferIndex() {
	currentBackBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
}

D3D12_RESOURCE_BARRIER* SwapChain::GetBackBufferTransitionBarrier(
	D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) {

	// transitionの設定
	barrier_.Transition.pResource   = resource_[currentBackBufferIndex_].Get();
	barrier_.Transition.StateBefore = stateBefore;
	barrier_.Transition.StateAfter  = stateAfter;

	return &barrier_;
}