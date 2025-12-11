#include "DxSwapChain.h"
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// SwapChain class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SwapChain::Init(
	Device* device, DescriptorHeaps* descriptorHeaps, CommandContext* command,
	DXGI_FORMAT format, const Vector2ui& size, const HWND& hwnd) {

	CreateSwapChain(device, command, format, size, hwnd);
	CreateRenderTargetView(device, descriptorHeaps, format, true);

}

void SwapChain::Term() {
	for (uint32_t i = 0; i < kBufferCount_; ++i) {
		descriptorsRTV_[i].Delete();
	}
}

void SwapChain::Present() {

	UINT syncInterval = {};
	UINT flags        = {};

	if (SxavengerConfig::GetConfig().isTearingAllowed && SxavengerConfig::GetSupport().isSupportTearing) {
		syncInterval = 0;
		flags        = DXGI_PRESENT_ALLOW_TEARING;

	} else {
		syncInterval = 1;
		flags        = 0;
	}

	swapChain_->Present(syncInterval, flags);
}

UINT SwapChain::GetCurrentBackBufferIndex() const {
	return swapChain_->GetCurrentBackBufferIndex();
}

void SwapChain::SetColorSpace(const DXGI_OUTPUT_DESC1& desc) {

	auto current = GetColorSpace(desc);

	if (current == colorSpace_) {
		return; // 変更なし
	}

	colorSpace_ = current.value();

	// 色空間の設定
	auto hr = swapChain_->SetColorSpace1(desc.ColorSpace);
	DxObject::Assert(hr, L"Color Space setting failed.");

	DXGI_HDR_METADATA_HDR10 metadata = {};

	switch (colorSpace_) {
		case ColorSpace::Rec_709:
			metadata.RedPrimary[0]   = GetChromaticity(0.640);
			metadata.RedPrimary[1]   = GetChromaticity(0.330);
			metadata.BluePrimary[0]  = GetChromaticity(0.300);
			metadata.BluePrimary[1]  = GetChromaticity(0.600);
			metadata.GreenPrimary[0] = GetChromaticity(0.150);
			metadata.GreenPrimary[1] = GetChromaticity(0.060);
			metadata.WhitePoint[0]   = GetChromaticity(0.3127);
			metadata.WhitePoint[1]   = GetChromaticity(0.3290);

			metadata.MaxMasteringLuminance = UINT(desc.MaxLuminance * 10000);
			metadata.MinMasteringLuminance = UINT(desc.MinLuminance * 0.001);

			metadata.MaxContentLightLevel = 100;
			break;

		case ColorSpace::Rec_2020_1000nit:
			metadata.RedPrimary[0]   = GetChromaticity(0.708);
			metadata.RedPrimary[1]   = GetChromaticity(0.292);
			metadata.BluePrimary[0]  = GetChromaticity(0.170);
			metadata.BluePrimary[1]  = GetChromaticity(0.797);
			metadata.GreenPrimary[0] = GetChromaticity(0.131);
			metadata.GreenPrimary[1] = GetChromaticity(0.046);
			metadata.WhitePoint[0]   = GetChromaticity(0.3127);
			metadata.WhitePoint[1]   = GetChromaticity(0.3290);

			metadata.MaxMasteringLuminance = UINT(desc.MaxLuminance * 10000);
			metadata.MinMasteringLuminance = UINT(desc.MinLuminance * 0.001);

			metadata.MaxContentLightLevel = 1000;
			break;

		case ColorSpace::Rec_2020_2000nit:
			metadata.RedPrimary[0]   = GetChromaticity(0.708);
			metadata.RedPrimary[1]   = GetChromaticity(0.292);
			metadata.BluePrimary[0]  = GetChromaticity(0.170);
			metadata.BluePrimary[1]  = GetChromaticity(0.797);
			metadata.GreenPrimary[0] = GetChromaticity(0.131);
			metadata.GreenPrimary[1] = GetChromaticity(0.046);
			metadata.WhitePoint[0]   = GetChromaticity(0.3127);
			metadata.WhitePoint[1]   = GetChromaticity(0.3290);

			metadata.MaxMasteringLuminance = UINT(desc.MaxLuminance * 10000);
			metadata.MinMasteringLuminance = UINT(desc.MinLuminance * 0.001);

			metadata.MaxContentLightLevel = 2000;
			break;
	}

	hr = swapChain_->SetHDRMetaData(
		DXGI_HDR_METADATA_TYPE_HDR10, sizeof(metadata), &metadata
	);
	DxObject::Assert(hr, L"HDR metadata setting failed.");

	Logger::EngineLog(std::format("[DXOBJECT SwapChain] color space changed: {}", magic_enum::enum_name(colorSpace_)));
}

D3D12_RESOURCE_BARRIER SwapChain::GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) const {

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = resources_[GetCurrentBackBufferIndex()].Get();
	barrier.Transition.StateBefore = stateBefore;
	barrier.Transition.StateAfter  = stateAfter;

	return barrier;
}

std::optional<SwapChain::ColorSpace> SwapChain::GetColorSpace(const DXGI_OUTPUT_DESC1& desc) {
	switch (desc.ColorSpace) {

		case DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709:
			return ColorSpace::Rec_709;

		case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
		case DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020:
			return desc.MaxLuminance > 1000.0f ? ColorSpace::Rec_2020_2000nit : ColorSpace::Rec_2020_1000nit;

		default:
			return std::nullopt;
	}
}

UINT16 SwapChain::GetChromaticity(double v) {
	return static_cast<UINT16>(v * 50000);
}

void SwapChain::CreateSwapChain(Device* device, CommandContext* command, DXGI_FORMAT format, const Vector2ui& size, const HWND& hwnd) {

	// スワップチェインを生成
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width            = size.x;
	desc.Height           = size.y;
	desc.Format           = format;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount      = kBufferCount_;
	desc.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags            = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	desc.Scaling          = DXGI_SCALING_NONE;

	auto hr = device->GetFactory()->CreateSwapChainForHwnd(
		command->GetCommandQueue(),
		hwnd,
		&desc,
		nullptr, nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf())
	);
	DxObject::Assert(hr, L"swap chain create failed.");

}

void SwapChain::CreateRenderTargetView(Device* device, DescriptorHeaps* descriptorHeaps, DXGI_FORMAT format, bool isSRGB) {

	// swap chainからresourceを引っ張ってくる
	for (uint32_t i = 0; i < kBufferCount_; ++i) {
		auto hr = swapChain_->GetBuffer(
			i, IID_PPV_ARGS(&resources_[i])
		);
		DxObject::Assert(hr, L"swap chain get buffer failed.");
	}

	if (isSRGB) {
		if (format == DxObject::ConvertToSRGB(format)) {
			Logger::EngineLog("[DXOBJECT SwapChain] warning | SRGB format is not found.");
		}

		format = DxObject::ConvertToSRGB(format);
	}

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format        = format;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (uint32_t i = 0; i < kBufferCount_; ++i) {
		descriptorsRTV_[i] = descriptorHeaps->GetDescriptor(DescriptorType::kDescriptor_RTV);

		device->GetDevice()->CreateRenderTargetView(
			resources_[i].Get(), &desc, descriptorsRTV_[i].GetCPUHandle()
		);
	}
}
