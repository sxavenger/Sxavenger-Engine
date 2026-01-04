#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Sxl/Flag.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FBaseTexture {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Flag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Flag: uint8_t{
		None            = 0,
		RenderTarget    = 1 << 0,
		UnorderedAccess = 1 << 1,

		All = RenderTarget | UnorderedAccess
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FBaseTexture() = default;
	FBaseTexture(const Vector2ui& size, DXGI_FORMAT format, Sxl::Flag<Flag> flag) { Create(size, format, flag); }
	virtual ~FBaseTexture() { Term(); }

	void Create(const Vector2ui& size, DXGI_FORMAT format, Sxl::Flag<Flag> flag);

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

	const DxObject::Descriptor& GetDescriptorSRV() const { return descriptorSRV_; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const DXGI_FORMAT& GetFormat() const { return format_; }

	static const D3D12_RESOURCE_STATES GetDefaultState() { return kDefaultState_; }

	const Vector2ui& GetSize() const { return size_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directx12 *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorUAV_;
	DxObject::Descriptor descriptorSRV_;
	DxObject::Descriptor descriptorRTV_;

	//* parameter *//

	Vector2ui size_;

	DXGI_FORMAT format_;

	Sxl::Flag<Flag> flag_ = Flag::None;

	//* default state *//

	static inline constexpr D3D12_RESOURCE_STATES kDefaultState_ = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

	//=========================================================================================
	// private methods
	//=========================================================================================

	D3D12_RESOURCE_FLAGS GetResourceFlags() const;

	std::optional<D3D12_CLEAR_VALUE> GetClearValue() const;

};

SXAVENGER_ENGINE_NAMESPACE_END
