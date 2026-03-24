#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Color4.h>
#include <Lib/Sxl/Flag.h>

//* c++
#include <optional>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderTexture final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Flag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Flag : uint8_t {
		None            = 0,
		RenderTarget    = 1 << 0,
		UnorderedAccess = 1 << 1,
		ShaderResource  = 1 << 2,

		All = RenderTarget | UnorderedAccess | ShaderResource
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

		bool Compatible(const Option& other) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui   resolution = {};
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

		Sxl::Flag<Flag> flag = Flag::All;
		Color4f clearColor   = {};

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderTexture() = default;
	FRenderTexture(const Vector2ui& resolution, DXGI_FORMAT format, Sxl::Flag<Flag> flag = Flag::All, Color4f clearColor = {}) { Create({ .resolution = resolution, .format = format, .flag = flag, .clearColor = clearColor }); }

	void Create(const Option& option);

	void Reset();

	void SetName(const std::wstring& name) const;
	void SetName(const std::string& name) const;

	//* transition option *//

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionState(D3D12_RESOURCE_STATES state);
	void SetTransitionState(std::vector<D3D12_RESOURCE_BARRIER>& barriers, D3D12_RESOURCE_STATES state);
	void TransitionState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state);

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionDefaultState();
	void SetTransitionDefaultState(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	void TransitionDefaultState(const DirectXQueueContext* context);

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionRenderTarget();
	void SetTransitionRenderTarget(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	void TransitionRenderTarget(const DirectXQueueContext* context);

	void ClearRenderTarget(const DirectXQueueContext* context);

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionUnorderedAccess();
	void SetTransitionUnorderedAccess(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	void TransitionUnorderedAccess(const DirectXQueueContext* context);

	void BarrierUnorderedAccess(const DirectXQueueContext* context);

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const DxObject::Descriptor& GetDescriptorRTV() const;
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const;

	const DxObject::Descriptor& GetDescriptorSRV() const;
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	const DxObject::Descriptor& GetDescriptorUAV() const;
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const;

	DXGI_FORMAT GetFormat() const { return option_.format; }

	const Vector2ui& GetResolution() const { return option_.resolution; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	//* DirectX12 *//

	DxObject::Resource resource_;

	DxObject::Descriptor descriptorRTV_;
	DxObject::Descriptor descriptorUAV_;
	DxObject::Descriptor descriptorSRV_;

	//* parameter *//

	Option option_;

	//* default state *//

	static inline constexpr D3D12_RESOURCE_STATES kDefaultState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource(const Option& option);
	void CreateDescriptor(const Option& option);

};

SXAVENGER_ENGINE_NAMESPACE_END
