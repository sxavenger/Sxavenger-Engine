#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneDepth class
////////////////////////////////////////////////////////////////////////////////////////////
class FSceneDepth { //!< 深度バッファ管理クラス
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSceneDepth()  = default;
	~FSceneDepth() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* option *//

	void TransitionBeginRasterizer(const DirectXThreadContext* context) const;

	void TransitionEndRasterizer(const DirectXThreadContext* context) const;

	void ClearRasterizerDepth(const DirectXThreadContext* context) const;

	void TransitionBeginRaytracing(const DirectXThreadContext* context) const;

	void TransitionEndRaytracing(const DirectXThreadContext* context) const;

	// todo: raytracing -> rasterizer

	void TransitionBeginStateRasterizer(const DirectXThreadContext* context, D3D12_RESOURCE_STATES state) const;

	void TransitionEndStateRasterizer(const DirectXThreadContext* context, D3D12_RESOURCE_STATES state) const;

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRasterizerCPUHandleDSV() const { return rasterizer_.descriptorDSV_.GetCPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetRasterizerGPUHandleSRV() const { return rasterizer_.descriptorSRV_.GetGPUHandle(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetRaytracingGPUHandleUAV() const { return raytracing_.descriptorUAV_.GetGPUHandle(); }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RasterizerDepth structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RasterizerDepth {

		//* member *//

		ComPtr<ID3D12Resource> resource_;
		DxObject::Descriptor descriptorDSV_;
		DxObject::Descriptor descriptorSRV_;

		//* method *//

		void Create(const Vector2ui& size); //!< Format is default depth format only.

		void Delete();

		void BeginWrite(const DirectXThreadContext* context) const; //!< Transition to depth write state.

		void EndWrite(const DirectXThreadContext* context) const; //!< Transition to pixel shader resource state.

		void ClearDepth(const DirectXThreadContext* context) const; //!< Clear depth value.

		void BeginState(const DirectXThreadContext* context, D3D12_RESOURCE_STATES state) const; //!< Transition to user state

		void EndState(const DirectXThreadContext* context, D3D12_RESOURCE_STATES state) const; //!< Transition to pixel shader resource state.

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RaytracingDepth structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RaytracingDepth {

		//* member *//

		ComPtr<ID3D12Resource> resource_;
		DxObject::Descriptor descriptorUAV_;

		//* method *//

		void Create(const Vector2ui& size); //!< Format is default depth compatible format only.

		void Delete();

		void BeginWrite(const DirectXThreadContext* context) const; //!< Transition to unordered access state.

		void EndWrite(const DirectXThreadContext* context) const; //!< Transition to pixel shader resource state.

	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	RasterizerDepth rasterizer_;
	RaytracingDepth raytracing_;

	// todo: 深度プリパスの追加
};
