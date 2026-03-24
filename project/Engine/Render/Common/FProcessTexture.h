#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FRenderTexture.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FProcessTexture final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Argument structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Argument {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		bool Compatible(const Argument& other) const;

		uint16_t GetMiplevel() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui resolution = {};
		DXGI_FORMAT format   = DxObject::kDefaultOffscreenFormat;

		std::optional<uint16_t> maxMiplevel = std::nullopt; //!< miplevelの最大値.
		//! [std::nullopt]: 最大miplevel
		//! [value]: 指定されたmiplevelまで生成

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(const Argument& argument);

	void SetName(const std::wstring& name) const;

	//* transition option *//

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionState(D3D12_RESOURCE_STATES state);
	void SetTransitionState(std::vector<D3D12_RESOURCE_BARRIER>& barriers, D3D12_RESOURCE_STATES state);
	void TransitionState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state);

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionDefaultState();
	void SetTransitionDefaultState(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	void TransitionDefaultState(const DirectXQueueContext* context);

	NODISCARD std::optional<D3D12_RESOURCE_BARRIER> GetTransitionUnorderedAccess();
	void SetTransitionUnorderedAccess(std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	void TransitionUnorderedAccess(const DirectXQueueContext* context);

	void BarrierUnorderedAccess(const DirectXQueueContext* context);

	//* process option *//

	void GenerateMipmap(const DirectXQueueContext* context, const std::optional<uint16_t>& miplevel = std::nullopt);

	//! @brief [Render Texture] => [Process Texture] のコピー.
	void ImportFromRenderTexture(const DirectXQueueContext* context, FRenderTexture* texture);

	//! @brief [Process Texture] => [Render Texture] のコピー.
	void ExportToRenderTexture(const DirectXQueueContext* context, FRenderTexture* texture);

	//* getter *//

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const DxObject::Descriptor& GetDescriptorSRV() const;
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	const DxObject::Descriptor& GetDescriptorUAV(uint16_t miplevel = 0) const;
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV(uint16_t miplevel = 0) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *//

	DxObject::Resource resource_;

	DxObject::Descriptor descriptorSRV_;
	std::vector<DxObject::Descriptor> descriptorsUAV_;

	//* argument *//

	Argument argument_;

	//* default state *//

	static inline constexpr D3D12_RESOURCE_STATES kDefaultState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateResource(const Argument& argument);
	void CreateDescriptor(const Argument& argument);

};

SXAVENGER_ENGINE_NAMESPACE_END
