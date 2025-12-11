#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxDescriptor.h"
#include "DxDescriptorHeaps.h"
#include "DxCommandContext.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SwapChain class
////////////////////////////////////////////////////////////////////////////////////////////
class SwapChain {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ColorSpace enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class ColorSpace : uint32_t {
		Rec_709,
		Rec_2020_1000nit,
		Rec_2020_2000nit,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SwapChain()  = default;
	~SwapChain() { Term(); }

	void Init(
		Device* device, DescriptorHeaps* descriptorHeaps, CommandContext* command,
		DXGI_FORMAT format, const Vector2ui& size, const HWND& hwnd
	);

	void Term();

	void Present();

	UINT GetCurrentBackBufferIndex() const;

	void SetColorSpace(const DXGI_OUTPUT_DESC1& desc);

	//* getter *//

	D3D12_RESOURCE_BARRIER GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) const;

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetBackBufferCPUHandle() const { return descriptorsRTV_[GetCurrentBackBufferIndex()].GetCPUHandle(); }

	const ColorSpace GetColorSpace() const { return colorSpace_; }

	static const UINT GetBufferCount() { return kBufferCount_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<IDXGISwapChain4> swapChain_;

	//* buffers *//

	static const UINT      kBufferCount_ = 2;
	ComPtr<ID3D12Resource> resources_[kBufferCount_];
	DxObject::Descriptor   descriptorsRTV_[kBufferCount_];

	//* parameter *//

	ColorSpace colorSpace_ = ColorSpace::Rec_709;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	static std::optional<ColorSpace> GetColorSpace(const DXGI_OUTPUT_DESC1& desc);

	static UINT16 GetChromaticity(double v);

	//* create methods *//

	void CreateSwapChain(Device* device, CommandContext* command, DXGI_FORMAT format, const Vector2ui& size, const HWND& hwnd);

	void CreateRenderTargetView(Device* device, DescriptorHeaps* descriptorHeaps, DXGI_FORMAT format, bool isSRGB);
};

DXOBJECT_NAMESPACE_END
