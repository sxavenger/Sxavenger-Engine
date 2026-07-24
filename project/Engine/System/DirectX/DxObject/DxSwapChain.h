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

	////////////////////////////////////////////////////////////////////////////////////////////
	// Buffer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Buffer {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Reset();

		//=========================================================================================
		// public variables
		//=========================================================================================

		ComPtr<ID3D12Resource> resource;
		DxObject::Descriptor descriptorRTV;

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

	void Resize(
		Device* device, DescriptorHeaps* descriptorHeaps,
		DXGI_FORMAT format, const Vector2ui& size
	);

	void Term();

	void Present();

	UINT GetCurrentBackBufferIndex() const;

	void SetColorSpace(const DXGI_OUTPUT_DESC1& desc);

	//* getter *//

	D3D12_RESOURCE_BARRIER GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) const;

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetBackBufferCPUHandle() const;

	const ColorSpace GetColorSpace() const { return colorSpace_; }

	static const UINT GetBufferCount() { return kBufferCount; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<IDXGISwapChain4> swapChain_;

	//* buffers *//

	static const UINT kBufferCount = 2;
	std::array<Buffer, kBufferCount> buffers_;

	//* parameter *//

	ColorSpace colorSpace_ = ColorSpace::Rec_709;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	static std::optional<ColorSpace> GetColorSpace(const DXGI_OUTPUT_DESC1& desc);

	static UINT16 GetChromaticity(double v);

	//* helper methods *//

	void CreateSwapChain(Device* device, CommandContext* command, DXGI_FORMAT format, const Vector2ui& size, const HWND& hwnd);
	void ResizeSwapChain(DXGI_FORMAT format, const Vector2ui& size);

	void CreateBuffer(Device* device, DescriptorHeaps* descriptorHeaps, DXGI_FORMAT format, bool isSRGB);
};

DXOBJECT_NAMESPACE_END
