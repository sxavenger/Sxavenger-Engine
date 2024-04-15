#include "DxrSwapChain.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// WinApp
#include <WinApp.h>

// DxObject
#include <DxrDevices.h>
#include <DxrCommand.h>
#include <DxrDescriptorHeaps.h>

#include <Logger.h>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////////////////
// SwapChain methods
////////////////////////////////////////////////////////////////////////////////////////////

DxrObject::SwapChain::SwapChain(
	Devices* devices, Command* command, DescriptorHeaps* descriptorHeaps,
	WinApp* winApp, int32_t clientWidth, int32_t clientHeight) {

	Init(devices, command, descriptorHeaps, winApp, clientWidth, clientHeight);
}

DxrObject::SwapChain::~SwapChain() { Term(); }

void DxrObject::SwapChain::Init(
	Devices* devices, Command* command, DescriptorHeaps* descriptorHeaps,
	WinApp* winApp, int32_t clientWidth, int32_t clientHeight) {

	ID3D12Device* device = devices->GetDevice();

	// スワップチェインを生成
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width            = clientWidth;
		desc.Height           = clientHeight;
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

		assert(SUCCEEDED(hr));
		Log("[DxrObject::SwapChain]: swapChain_ << Complete Create \n");
	}

	// SwapChainからResourceを引っ張ってくる
	{
		for (uint32_t i = 0; i < kBufferCount_; ++i) {
			auto hr = swapChain_->GetBuffer(
				i, IID_PPV_ARGS(&swapChainResource_[i])
			);

			assert(SUCCEEDED(hr));
		}

		Log("[DxrObject::SwapChain]: swapChainResource_[..] << Complete Create \n");
	}

	// RTVの設定
	{
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		for (uint32_t i = 0; i < kBufferCount_; ++i) {
			uint32_t descriptorIndex = descriptorHeaps->GetDescriptorCurrentIndex(DescriptorType::RTV);

			handleCPU_RTV_[i]
				= descriptorHeaps->GetCPUDescriptorHandle(DescriptorType::RTV, descriptorIndex);

			device->CreateRenderTargetView(
				swapChainResource_[i].Get(), &desc, handleCPU_RTV_[i]
			);
		}
	}

	// barrierの設定
	{
		barrier_.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	}
}

void DxrObject::SwapChain::Term() {
}

void DxrObject::SwapChain::Present(UINT SyncInterval, UINT Flags) {
	swapChain_->Present(SyncInterval, Flags);
}

D3D12_RESOURCE_BARRIER* DxrObject::SwapChain::GetTransitionBarrier(
	UINT backBufferIndex,
	D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) {

	// transitionの設定
	barrier_.Transition.pResource   = swapChainResource_[backBufferIndex].Get();
	barrier_.Transition.StateBefore = stateBefore;
	barrier_.Transition.StateAfter  = stateAfter;

	return &barrier_;
}