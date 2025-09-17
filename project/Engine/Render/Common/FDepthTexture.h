#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FDepthTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FDepthTexture {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// FRasterizerDepth structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct FRasterizerDepth {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		FRasterizerDepth()  = default;
		~FRasterizerDepth() = default;

		void Create(const Vector2ui& size);
		//!< Format is default depth format only.

		void Delete();

		//!< Transition to depth write state.
		void BeginWrite(const DirectXQueueContext* context) const;

		//!< Transition to default resource state.
		void EndWrite(const DirectXQueueContext* context) const;

		//!< Clear depth value.
		void ClearDepth(const DirectXQueueContext* context) const;

		//!< Transition to user state
		void BeginState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const;

		//!< Transition to default resource state.
		void EndState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		ComPtr<ID3D12Resource> resource_;
		DxObject::Descriptor descriptorDSV_;
		DxObject::Descriptor descriptorSRV_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// FRaytracingDepth structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct FRaytracingDepth {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Create(const Vector2ui& size); //!< Format is default depth compatible format only.

		void Delete();

		//!< Transition to unordered access state.
		void BeginWrite(const DirectXQueueContext* context) const;

		//!< Transition to pixel shader resource state.
		void EndWrite(const DirectXQueueContext* context) const;

		//!< Transition to user state
		void BeginState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const;

		//!< Transition to default resource state.
		void EndState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		ComPtr<ID3D12Resource> resource_;
		DxObject::Descriptor descriptorUAV_;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FDepthTexture() = default;
	~FDepthTexture() { Term(); }

	//* option *//

	void Create(const Vector2ui& size);

	void Term();

	//* transition option *//

	void TransitionBeginRasterizer(const DirectXQueueContext* context) const;

	void TransitionEndRasterizer(const DirectXQueueContext* context) const;

	void ClearRasterizerDepth(const DirectXQueueContext* context) const;

	void TransitionBeginRaytracing(const DirectXQueueContext* context) const;

	void TransitionEndRaytracing(const DirectXQueueContext* context) const;

	void TransitionBeginStateRasterizer(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const;

	void TransitionEndStateRasterizer(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) const;

	// todo: raytracing -> rasterizer
	void CopyRaytracingToRasterizer(const DirectXQueueContext* context) const;

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRasterizerCPUHandleDSV() const { return rasterizer_.descriptorDSV_.GetCPUHandle(); }

	const DxObject::Descriptor& GetRasterizerDescriptorSRV() const { return rasterizer_.descriptorSRV_; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetRasterizerGPUHandleSRV() const { return rasterizer_.descriptorSRV_.GetGPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetRaytracingGPUHandleUAV() const { return raytracing_.descriptorUAV_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* depth *//

	FRasterizerDepth rasterizer_;
	FRaytracingDepth raytracing_;

	//* state *//

	static const D3D12_RESOURCE_STATES kDefaultResourceState_ = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

};
