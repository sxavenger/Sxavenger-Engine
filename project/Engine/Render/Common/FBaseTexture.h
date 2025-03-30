#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FBaseTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FBaseTexture() = default;
	virtual ~FBaseTexture() { Term(); }

	void Term();

	//* transition *//

	D3D12_RESOURCE_BARRIER TransitionBeginRenderTarget() const;
	void TransitionBeginRenderTarget(const DirectXThreadContext* context) const;

	D3D12_RESOURCE_BARRIER TransitionEndRenderTarget() const;
	void TransitionEndRenderTarget(const DirectXThreadContext* context) const;

	void ClearRenderTarget(const DirectXThreadContext* context) const;

	D3D12_RESOURCE_BARRIER TransitionBeginUnordered() const;
	void TransitionBeginUnordered(const DirectXThreadContext* context) const;

	D3D12_RESOURCE_BARRIER TransitionEndUnordered() const;
	void TransitionEndUnordered(const DirectXThreadContext* context) const;

	//!< need user state function...?

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const { return descriptorRTV_.GetCPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const { return descriptorUAV_.GetGPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const DXGI_FORMAT& GetFormat() const { return format_; }

	static const D3D12_RESOURCE_STATES GetDefaultState() { return kDefaultState_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* directx12 *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorUAV_;
	DxObject::Descriptor descriptorSRV_;
	DxObject::Descriptor descriptorRTV_;

	//* parameter *//

	DXGI_FORMAT format_;

	//* default state *//

	static inline constexpr D3D12_RESOURCE_STATES kDefaultState_ = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

};
