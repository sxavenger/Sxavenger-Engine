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

//* engine
#include <Engine/System/Window/Window.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SwapChain class
////////////////////////////////////////////////////////////////////////////////////////////
class SwapChain {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SwapChain()  = default;
	~SwapChain() { Term(); }

	void Init(
		Device* device, DescriptorHeaps* descriptorHeaps, CommandContext* command,
		Window* window
	);

	void Term();

	void Present();

	UINT GetCurrentBackBufferIndex();

	//* getter *//

	D3D12_RESOURCE_BARRIER GetBackBufferTransitionBarrier(D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) const;

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetBackBufferCPUHandle() const { return descriptorsRTV_[currentBackBufferIndex_].GetCPUHandle(); }

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

	UINT currentBackBufferIndex_ = 0;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateSwapChain(Device* device, CommandContext* command, Window* window);
	void CreateRTV(Device* device, DescriptorHeaps* descriptorHeaps);
};

_DXOBJECT_NAMESPACE_END
