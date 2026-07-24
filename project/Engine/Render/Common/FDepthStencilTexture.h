#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Math/Vector2.h>

//* c++
#include <optional>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FDepthStencilTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FDepthStencilTexture {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Option structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Option {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		bool Compatible(const Option& other) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui resolution = {};

		DXGI_FORMAT format = DxObject::kDefaultDepthFormat;

		float clearDepth     = 1.0f;
		uint8_t clearStencil = 0;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FDepthStencilTexture() = default;
	FDepthStencilTexture(const Vector2ui& resolution) { Create({ .resolution = resolution }); }

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

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionDepthWrite();
	void SetTransitionDepthWrite(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	void TransitionDepthWrite(const DirectXQueueContext* context);

	void ClearDepthStencil(const DirectXQueueContext* context);

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionDepthRead();
	void SetTransitionDepthRead(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	void TransitionDepthRead(const DirectXQueueContext* context);

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const DxObject::Descriptor& GetDescriptorDSV() const;
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleDSV() const;

	const DxObject::Descriptor& GetDescriptorSRV() const;
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	DxObject::Resource resource_;

	DxObject::Descriptor descriptorDSV_;
	DxObject::Descriptor descriptorSRV_;

	//* option *//

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
