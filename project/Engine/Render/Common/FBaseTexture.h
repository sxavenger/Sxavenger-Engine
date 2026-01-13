#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Color4.h>
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

	////////////////////////////////////////////////////////////////////////////////////////////
	// Option structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Option {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		D3D12_RESOURCE_FLAGS GetResourceFlags() const;

		std::optional<D3D12_CLEAR_VALUE> GetClearValue() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui   size;
		DXGI_FORMAT format;

		Sxl::Flag<Flag> flag = Flag::None;
		Color4f clearColor   = {};

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FBaseTexture() = default;
	FBaseTexture(const Vector2ui& size, DXGI_FORMAT format, Sxl::Flag<Flag> flag, Color4f clearColor = {}) { Create({ .size = size, .format = format, .flag = flag, .clearColor = clearColor }); }
	virtual ~FBaseTexture() { Term(); } 

	void Create(const Option& option);

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

	const DxObject::Descriptor& GetDescriptorSRV() const { return descriptorSRV_; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const;

	const DXGI_FORMAT& GetFormat() const { return option_.format; }

	static const D3D12_RESOURCE_STATES GetDefaultState() { return kDefaultState_; }

	const Vector2ui& GetSize() const { return option_.size; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorUAV_;
	DxObject::Descriptor descriptorSRV_;
	DxObject::Descriptor descriptorRTV_;

	//* parameter *//

	Option option_;

	//* default state *//

	static inline constexpr D3D12_RESOURCE_STATES kDefaultState_ = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

	//=========================================================================================
	// private methods
	//=========================================================================================

};

SXAVENGER_ENGINE_NAMESPACE_END
