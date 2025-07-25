#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

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

	void Create(const Vector2ui& size, DXGI_FORMAT format);

	void Term();

	//* transition *//

	D3D12_RESOURCE_BARRIER TransitionBeginRenderTarget() const;
	void TransitionBeginRenderTarget(const DirectXQueueContext* context) const;

	D3D12_RESOURCE_BARRIER TransitionEndRenderTarget() const;
	void TransitionEndRenderTarget(const DirectXQueueContext* context) const;

	void ClearRenderTarget(const DirectXQueueContext* context) const;

	D3D12_RESOURCE_BARRIER TransitionBeginUnordered() const;
	void TransitionBeginUnordered(const DirectXQueueContext* context) const;

	D3D12_RESOURCE_BARRIER TransitionEndUnordered() const;
	void TransitionEndUnordered(const DirectXQueueContext* context) const;

	D3D12_RESOURCE_BARRIER TransitionBeginState(D3D12_RESOURCE_STATES state) const;
	void TransitionBeginState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const;

	D3D12_RESOURCE_BARRIER TransitionEndState(D3D12_RESOURCE_STATES state) const;
	void TransitionEndState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const;

	void BarrierUAV(const DirectXQueueContext* context) const;

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
